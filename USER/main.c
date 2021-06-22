#include "main.h"
#include "sys.h"
#include "delay.h"
#include "config.h"
#include "m_PinGpio.h"
#include "malloc.h" 
#include "m_Serial.h"
#include "m_Gcode.h"
#include "u8g2_user.h"
#include "input_key.h"

#include "m_sd.h"
#include "ff.h"
#include "usbh_usr.h" 

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

extern u8g2_t u8g2;
extern TIM_HandleTypeDef htim8;

bool USB_IsConnected = false;   //U盘连接成功标志位

const char echomagic[] PROGMEM = "echo: ";
const char errormagic[] PROGMEM = "Error: ";

static long gcode_N, gcode_LastN; //处理行号的存储器

static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE]; //命令队列存储
static int bufindr = 0;  //当前读取队列的索引
static int bufindw = 0;  //当前写入队列的索引
static int buflen = 0;  //获取队列的长度
static char serial_char;  //串口获取的命令字符
static int serial_count = 0;  //串口获取的命令字符个数

static bool comment_mode = false; 
static char *strchr_pointer;  //在命令中查找字符的指针

static unsigned long previous_millis_cmd = 0;

bool Stopped=false;


//USB连接完成后执行程序
//返回值:0,正常
//       1,有问题
u8 USH_User_App(void)
{
	u32 total,free;
	u8 res=0;
	printf("设备连接成功!\r\n");
	f_mount(fs[1],"1:",1); 	//重新挂载U盘
	res=exf_getfree("1:",&total,&free);
	printf("res= %d\r\n",res);
	if(res==0)
	{
		printf("U Disk Total Size:%dMB\r\n",total>>10);
		printf("U Disk  Free Size:%dMB\r\n",free>>10);
	} 
	USB_IsConnected = 1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
	{	
		mf_scan_files("1:");
	}
	return res;
} 

//获取队列中的对应数值
float code_value(void)
{
  return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}

//获取队列中的对应数值
long code_value_long(void)
{
  return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

//检测队列中是否包含对应字符
bool code_seen(char code)
{
  strchr_pointer = strchr(cmdbuffer[bufindr], code);
  return (strchr_pointer != NULL);
}

int main(void)
{ 
	HAL_Init();                    	//初始化HAL库    
	Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	m_LED_Init();										//初始化LED灯
	my_mem_init();                  //初始化内存管理
	m_Serial_Init(BAUDRATE);        //初始化串口             
	SD_DETECT_Init();               //SD卡检测引脚初始化
	SD_IsDetected();
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);
	LCD12864_Init();
	Input_Encoder_Init();
	
  delay_ms(100);
	
	printf("初始化完成！\r\n");
	RUN_LED = 1;		 
	
	while(1)
	{
		//U盘事件处理
		if(!USB_IsConnected){
			//检测U盘是否插入；高速循环扫描（检测到U盘后可不用扫描）
			USBH_Process(&USB_OTG_Core, &USB_Host);
			delay_ms(1);
		}
		else{
			//处理U盘文件系统
			if(HCD_IsDeviceConnected(&USB_OTG_Core)){
				//处理U盘文件系统
			}
			else{
			//U盘拔出事件处理
				USB_IsConnected = 0;
				f_mount(0,"1:",1);  //卸载U盘
			}
		}
		
    if(buflen < (BUFSIZE-1)){
			m_Get_Command(); 
		}
		if(buflen){
			process_commands();
			//程序延时，防止测试时运行速度过快
			HAL_Delay(500);
			buflen = (buflen-1);
			bufindr = (bufindr + 1)%BUFSIZE;
		}
		if(menu_update_flage)
		{
			LCD12864_UpDate();
		}
	}
}

void m_Get_Command(void)
{
	uint32_t avalaibleBytes = m_UartGetNbRxAvalaibleBytes();
	while(avalaibleBytes > 0 && buflen < (BUFSIZE -1))
	{
		serial_char = m_UartGetNextRxBytes();
		avalaibleBytes--;
		if(serial_char == '\n' || serial_char == '\r' ||
		(serial_char == ':' && (comment_mode == false)) ||
		serial_count >= (MAX_CMD_SIZE - 1))
		{
			//空命令
			if(!serial_count)
			{
				comment_mode = false;
				return;
			}
			cmdbuffer[bufindw][serial_count] = 0;
			comment_mode = false;
			//检测是否含有行号；如果有进行校验
			if(strchr(cmdbuffer[bufindw], 'N') != NULL)
			{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'N');
					//获取该条命令的N后边的数值（行号）
					gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
					//命令的行号校验；如果校验不通过，则报错
					if(gcode_N != gcode_LastN+1 && (strstr(cmdbuffer[bufindw], PSTR("M110")) == NULL)) 
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_LINE_NO);
						SERIAL_ERRORLN(gcode_LastN);
						m_FlushSerialRequestResend();
						serial_count = 0;
						return;
					}
					//检测命令的行号与校验码是否匹配
					if(strchr(cmdbuffer[bufindw], '*') != NULL)
					{
						uint8_t checksum = 0;
						uint8_t count = 0;
						while(cmdbuffer[bufindw][count] != '*') checksum = checksum^cmdbuffer[bufindw][count++];
						strchr_pointer = strchr(cmdbuffer[bufindw], '*');

						if( (int)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum) 
						{
							SERIAL_ERROR_START;
							SERIAL_ERRORPGM(MSG_ERR_CHECKSUM_MISMATCH);
							SERIAL_ERRORLN(gcode_LastN);
							m_FlushSerialRequestResend();
							serial_count = 0;
							return;
						}
					}
					else
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_NO_CHECKSUM);
						SERIAL_ERRORLN(gcode_LastN);
						m_FlushSerialRequestResend();
						serial_count = 0;
						return;
					}
					gcode_LastN = gcode_N;
				}
				else  //如果没有检测到包含行号，但是有校验码
				{
					if((strchr(cmdbuffer[bufindw], '*') != NULL))
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
						SERIAL_ERRORLN(gcode_LastN);
						serial_count = 0;
						return;
					}
				}
				if((strchr(cmdbuffer[bufindw], 'G') != NULL))
				{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'G');
					switch((int)((strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)))){
					case 0:
					case 1:
					case 2:
					case 3:
						if (Stopped == true) {
						SERIAL_ERRORPGM(MSG_ERR_STOPPED);
						}
						break;
					default:
						break;
					}

				}
				//接收到急停命令
				if(strcmp(cmdbuffer[bufindw], "M112") == 0){
					//kill();
				}
//				printf(cmdbuffer[bufindw]);
//				printf("\r\n");
				//命令队列加一，准备接受下一条命令
				bufindw = (bufindw + 1)%BUFSIZE;
				buflen += 1;
				serial_count = 0;
		}
		else
		{
			if(serial_char == ';') comment_mode = true;
			if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
		}
	}
	m_UartIfSendQueuedData();
}

void process_commands()
{
	if(code_seen('G'))
	{
		switch((int)code_value())
		{
			case 0:
			case 1: G0_G1(); break;
		}
	}
	else if(code_seen('M'))
	{
		switch((int)code_value())
		{
			case 0:  break;
		}
	}
	else
	{
		SERIAL_ECHO_START;
		SERIAL_ECHOPGM(MSG_UNKNOWN_COMMAND);
		SERIAL_ECHO(cmdbuffer[bufindr]);
		SERIAL_ECHOLNPGM("\"");
	}
	
	ClearToSend();
}


void m_FlushSerialRequestResend(void)
{
  SERIAL_PROTOCOLPGM(MSG_RESEND);
  SERIAL_PROTOCOLLN(gcode_LastN + 1);
  ClearToSend();
}

void ClearToSend(void)
{
  previous_millis_cmd = millis();
  SERIAL_PROTOCOLLNPGM(MSG_OK);
}


