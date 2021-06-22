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

bool USB_IsConnected = false;   //U�����ӳɹ���־λ

const char echomagic[] PROGMEM = "echo: ";
const char errormagic[] PROGMEM = "Error: ";

static long gcode_N, gcode_LastN; //�����кŵĴ洢��

static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE]; //������д洢
static int bufindr = 0;  //��ǰ��ȡ���е�����
static int bufindw = 0;  //��ǰд����е�����
static int buflen = 0;  //��ȡ���еĳ���
static char serial_char;  //���ڻ�ȡ�������ַ�
static int serial_count = 0;  //���ڻ�ȡ�������ַ�����

static bool comment_mode = false; 
static char *strchr_pointer;  //�������в����ַ���ָ��

static unsigned long previous_millis_cmd = 0;

bool Stopped=false;


//USB������ɺ�ִ�г���
//����ֵ:0,����
//       1,������
u8 USH_User_App(void)
{
	u32 total,free;
	u8 res=0;
	printf("�豸���ӳɹ�!\r\n");
	f_mount(fs[1],"1:",1); 	//���¹���U��
	res=exf_getfree("1:",&total,&free);
	printf("res= %d\r\n",res);
	if(res==0)
	{
		printf("U Disk Total Size:%dMB\r\n",total>>10);
		printf("U Disk  Free Size:%dMB\r\n",free>>10);
	} 
	USB_IsConnected = 1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
		mf_scan_files("1:");
	}
	return res;
} 

//��ȡ�����еĶ�Ӧ��ֵ
float code_value(void)
{
  return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}

//��ȡ�����еĶ�Ӧ��ֵ
long code_value_long(void)
{
  return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

//���������Ƿ������Ӧ�ַ�
bool code_seen(char code)
{
  strchr_pointer = strchr(cmdbuffer[bufindr], code);
  return (strchr_pointer != NULL);
}

int main(void)
{ 
	HAL_Init();                    	//��ʼ��HAL��    
	Stm32_Clock_Init(336,8,2,7);  	//����ʱ��,168Mhz
	delay_init(168);               	//��ʼ����ʱ����
	m_LED_Init();										//��ʼ��LED��
	my_mem_init();                  //��ʼ���ڴ����
	m_Serial_Init(BAUDRATE);        //��ʼ������             
	SD_DETECT_Init();               //SD��������ų�ʼ��
	SD_IsDetected();
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);
	LCD12864_Init();
	Input_Encoder_Init();
	
  delay_ms(100);
	
	printf("��ʼ����ɣ�\r\n");
	RUN_LED = 1;		 
	
	while(1)
	{
		//U���¼�����
		if(!USB_IsConnected){
			//���U���Ƿ���룻����ѭ��ɨ�裨��⵽U�̺�ɲ���ɨ�裩
			USBH_Process(&USB_OTG_Core, &USB_Host);
			delay_ms(1);
		}
		else{
			//����U���ļ�ϵͳ
			if(HCD_IsDeviceConnected(&USB_OTG_Core)){
				//����U���ļ�ϵͳ
			}
			else{
			//U�̰γ��¼�����
				USB_IsConnected = 0;
				f_mount(0,"1:",1);  //ж��U��
			}
		}
		
    if(buflen < (BUFSIZE-1)){
			m_Get_Command(); 
		}
		if(buflen){
			process_commands();
			//������ʱ����ֹ����ʱ�����ٶȹ���
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
			//������
			if(!serial_count)
			{
				comment_mode = false;
				return;
			}
			cmdbuffer[bufindw][serial_count] = 0;
			comment_mode = false;
			//����Ƿ����кţ�����н���У��
			if(strchr(cmdbuffer[bufindw], 'N') != NULL)
			{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'N');
					//��ȡ���������N��ߵ���ֵ���кţ�
					gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
					//������к�У�飻���У�鲻ͨ�����򱨴�
					if(gcode_N != gcode_LastN+1 && (strstr(cmdbuffer[bufindw], PSTR("M110")) == NULL)) 
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_LINE_NO);
						SERIAL_ERRORLN(gcode_LastN);
						m_FlushSerialRequestResend();
						serial_count = 0;
						return;
					}
					//���������к���У�����Ƿ�ƥ��
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
				else  //���û�м�⵽�����кţ�������У����
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
				//���յ���ͣ����
				if(strcmp(cmdbuffer[bufindw], "M112") == 0){
					//kill();
				}
//				printf(cmdbuffer[bufindw]);
//				printf("\r\n");
				//������м�һ��׼��������һ������
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


