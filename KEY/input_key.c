#include "fattester.h"
#include "exfuns.h"
#include "ff.h"
#include "malloc.h"
#include "input_key.h"
#include "u8g2_user.h"
#include "m_Serial.h"
#include "m_pin.h"
#include "string.h"
#include "delay.h"


TIM_HandleTypeDef TIM7_Handler;      //定时器7句柄

uint8_t menu_show;     			    //0: 显示主菜单画面地址寄存器
uint8_t control_menu_show;      //1: 显示控制画面地址寄存器

int8_t menu_current = 0;        //设置当前菜单光标位置
uint8_t menu_update_flage;      //屏幕刷新标志位

u8 Encoder_KEY_nu = 0;          //编码器按键相消抖计数
u8 Encoder_KEY_flage = 0;       //编码器按键相释放抬起标记位
u8 Encoder_AB_nu = 0;           //编码器旋转相消抖计数
u8 Encoder_AB_flage = 0;        //编码器旋转相消抖标记位

u8 Beep_IsRun = 0;              //按键蜂鸣器响应标志
u8 Beep_Run_falge = 0;          //蜂鸣器运行标志位
u8 Beep_RunTimeNum = 0;         //蜂鸣器运行时间寄存器（单位：2ms）

char SD_path[255] = {0};

//按键消抖用基本定时器7中断初始化
//2ms产生一次中断,用以检测按键按下的时间
void TIM7_Init(void)
{
	TIM7_Handler.Instance=TIM7;             //基本定时器6
	TIM7_Handler.Init.Prescaler=840-1;      //分频系数
	TIM7_Handler.Init.Period=200-1;         //自动装载值
	HAL_TIM_Base_Init(&TIM7_Handler);
    
	HAL_TIM_Base_Start_IT(&TIM7_Handler); //使能定时器7和定时器7更新中断：TIM_IT_UPDATE
}


void Input_Encoder_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOF_CLK_ENABLE();               //开启GPIOF时钟
	__HAL_RCC_GPIOG_CLK_ENABLE();               //开启GPIOG时钟
	
	//初始化旋转编码器A相脉冲引脚
	GPIO_InitStruct.Pin=BTN_A_Pin;                			
	GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;      //下降沿触发外部中断
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(BTN_A_PIN_Port,&GPIO_InitStruct);
	
	//初始化旋转编码器B相脉冲引脚
	GPIO_InitStruct.Pin=BTN_B_Pin;                			
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;                  //引脚输入模式
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(BTN_B_PIN_Port,&GPIO_InitStruct);
	
	//初始化蜂鸣器控制引脚
	GPIO_InitStruct.Pin = BEEPE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BEEPE_PIN_Port, &GPIO_InitStruct);	
	
	//设置中断线15_10
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,0);       //抢占优先级为2，子优先级为0
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线	
	
	//初始化按键检测的定时器7
	TIM7_Init();
}

//定时器底层驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM7)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		__HAL_RCC_GPIOA_CLK_ENABLE();           //开启编码器按键GPIOA时钟
		__HAL_RCC_TIM7_CLK_ENABLE();            //使能TIM7时钟
		
		//初始化旋转编码器按键
		GPIO_InitStruct.Pin=BTN_ENC_Pin;                			
		GPIO_InitStruct.Mode=GPIO_MODE_INPUT;                  //引脚输入模式
		GPIO_InitStruct.Pull=GPIO_PULLUP;
		HAL_GPIO_Init(BTN_ENC_PIN_Port,&GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(TIM7_IRQn,2,1);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //开启ITM3中断   
	}
}

//定时器7中断服务函数
void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM7_Handler);       //调用定时器中断处理公用函数
}

//外部中断10-15中断服务函数
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);		//调用外部中断处理公用函数
}

//定时器回调函数，定时器中断服务函数调用
//按键按下处理函数和按键消抖处理
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM7_Handler))
	{
		if(Encoder_KEY != 0){
			Encoder_KEY_nu = 0;        //清除按键抖动产生的计数
			Encoder_KEY_flage = 1;     //标记按键被释放
		}
		else{
			Encoder_KEY_nu++;
			//按键连续8次计数都被按下，且上一次确认按下已被释放
			if(Encoder_KEY_nu>=8 && Encoder_KEY_flage)
			{
				Encoder_KEY_flage = 0;     //标记按键被按下
				switch(menu_show)
				{
					//主菜单光标选择确认控制
					case 0:
						switch(menu_current)
						{
							case 0:
								menu_show = 1;      //显示控制界面菜单
								menu_current = 0;		//设置光标为初始位置
							break;
							case 1:
								menu_show = 2;      //系统设置控制界面菜单
								menu_current = 0;		//设置光标为初始位置
							break;
							case 2:
								menu_show = 3;      //SD卡菜单选择界面
								//写入SD卡根目录路径
								strcpy(SD_path, "0:");
								//获取根目录的文件列表
								mf_get_file_name((uint8_t *)SD_path);
								menu_current = 0;		//设置光标为初始位置
							break;
						}
					break;
						
					//控制界面光标选择确认控制
					case 1:
						switch(menu_current)
						{
							case 0:
								menu_show = 0;      //返回控制界面菜单
								menu_current = 0;		//设置光标为初始位置
							break;
						
						}	
					break;
					
					//系统设置界面光标选择确认控制
					case 2:
						switch(menu_current)
						{
							case 0:/*返回上一级*/
								menu_show = 0;      //返回控制界面菜单
								menu_current = 0;		//设置光标为初始位置
							break;
						
						}
					
					//SD卡界面光标选择确认控制
					case 3:
						//释放申请的内存
						for(u8 i=0; i<SD_file_un; i++)
						{
							myfree(SD_item[i]);
						}
						
						//返回SD卡文件上一层路径
						if(menu_current == 0)
						{
							char dest[100] = {0};
							//计算要截取上一层路径的位置
							char *a = strrchr(SD_path, '/');
							if(a)
							{
								//计算要截取的数据长度
								uint8_t date = strlen(SD_path) - strlen(a);
								//截取上一层文件路径
								memmove(dest, SD_path, date);
								//将路径覆盖到之前路径中
								strcpy(SD_path, dest);
								mf_get_file_name((uint8_t*)SD_path);
								printf("返回上一层文件夹！\r\n");
								printf("文件目录为：%s\r\n", SD_path);
								menu_current = 0;		//设置光标为初始位置
							}
							else
							{
								menu_show = 0;      //返回控制界面菜单
								printf("返回总菜单！\r\n");
								menu_current = 0;		//设置光标为初始位置
							}
						}
						//当前目录下的文件选择
						else
						{
							FRESULT res;
							//在该目录下查找对应的文件
							res = f_findfirst(&dir, &fileinfo, SD_path, SD_item[menu_current]);
							if(res == FR_OK && fileinfo.fname[0])
							{
								//判别该文件是否为一个文件夹
								if(fileinfo.fattrib&AM_DIR)
								{
									//文件目录拼接处理
									strcat(SD_path, "/");
									strcat(SD_path, SD_item[menu_current]);
									
									//获取新的目录下的文件列表
									mf_get_file_name((uint8_t*)SD_path);
									printf("该选项是一个文件夹！\r\n");
									printf("文件目录为：%s\r\n", SD_path);
									menu_current = 0;		//设置光标为初始位置
								}
								else
								{
									//选择的项目是文件
									printf("该选项是一个文件！\r\n");
									printf("文件名为：%s\r\n", fileinfo.fname);
								}
							}
							f_closedir(&dir);
						}
					break;
					
				}	
				Beep_IsRun = 1;
				
				//更新LCD12864显示菜单
				menu_update_flage = 1;
			}
		}
		
		//得到蜂鸣器按键响应命令
		if(Beep_IsRun == 1)
		{
			//启动蜂鸣器
			if(Beep_Run_falge == 0)
			{
				BEEP_ON;
				Beep_Run_falge = 1;
			}
			//记录蜂鸣器运行的单位时间次数
			Beep_RunTimeNum++;
			
			//到达蜂鸣器预设时间次数
			//关闭蜂鸣器、清除蜂鸣器各标志位
			if(Beep_RunTimeNum >= 15)
			{
				BEEP_OFF;
				Beep_IsRun = 0;
				Beep_Run_falge = 0;
				Beep_RunTimeNum = 0;
			}
		}
	}
}


//外部中断服务程序
//旋转编码开关处理函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		//旋转编码器旋钮被转动
		case GPIO_PIN_10:
			//第一次中断，并且A相是下降沿（编码器消抖）
			if(Encoder_A==0 && Encoder_AB_nu==0)
			{
				Encoder_AB_flage=0;
				if(Encoder_B == 1)     //根据B相，设定标志
					Encoder_AB_flage=1;
				Encoder_AB_nu=1;
			}
			
			//第二次中断，并且A相是上升沿（确认编码器转动，并判断方向）
			if(Encoder_A==1 && Encoder_AB_nu==1)
			{
				//编码器逆时针转动
				if(Encoder_B==0 && Encoder_AB_flage==1)
				{
					switch(menu_show)
					{
						//主菜单光标向下移动控制
						case 0:   //主界面菜单
							menu_current--;
							if(menu_current<0)
								menu_current = menu_main-1;
						break;
							
						case 1:  //控制界面菜单
							menu_current--;
							if(menu_current<0)
								menu_current = menu_control-1;
						break;
							
						case 2:  //系统设置界面菜单
							menu_current--;
							if(menu_current<0)
								menu_current = menu_setting-1;
						break;
						
						case 3:  //SD卡界面菜单
							menu_current--;
							if(menu_current<0)
								menu_current = SD_file_un-1;
						break;
					}
					
					//更新LCD12864显示菜单
					menu_update_flage = 1;
				}
				
				//编码器顺时针转动
				if(Encoder_B==1 && Encoder_AB_flage==0)
				{
					switch(menu_show)
					{
						//主菜单光标向下移动控制
						case 0:                     //主菜单
							menu_current++;
							if(menu_current>=menu_main)
								menu_current = 0;
						break;
							
						case 1:  //控制界面菜单
							menu_current++;
							if(menu_current>=menu_control)
								menu_current = 0;
						break;
						
						case 2:  //系统设置界面菜单
							menu_current++;
							if(menu_current>=menu_setting)
								menu_current = 0;
						break;
						
						case 3:  //SD卡界面菜单
							menu_current++;
							if(menu_current>=SD_file_un)
								menu_current = 0;
						break;							
					}
					
					//更新LCD12864显示菜单
					menu_update_flage = 1;
				}
				Encoder_AB_nu = 0;
			}
		break;
	}
}


