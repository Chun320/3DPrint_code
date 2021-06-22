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


TIM_HandleTypeDef TIM7_Handler;      //��ʱ��7���

uint8_t menu_show;     			    //0: ��ʾ���˵������ַ�Ĵ���
uint8_t control_menu_show;      //1: ��ʾ���ƻ����ַ�Ĵ���

int8_t menu_current = 0;        //���õ�ǰ�˵����λ��
uint8_t menu_update_flage;      //��Ļˢ�±�־λ

u8 Encoder_KEY_nu = 0;          //��������������������
u8 Encoder_KEY_flage = 0;       //�������������ͷ�̧����λ
u8 Encoder_AB_nu = 0;           //��������ת����������
u8 Encoder_AB_flage = 0;        //��������ת���������λ

u8 Beep_IsRun = 0;              //������������Ӧ��־
u8 Beep_Run_falge = 0;          //���������б�־λ
u8 Beep_RunTimeNum = 0;         //����������ʱ��Ĵ�������λ��2ms��

char SD_path[255] = {0};

//���������û�����ʱ��7�жϳ�ʼ��
//2ms����һ���ж�,���Լ�ⰴ�����µ�ʱ��
void TIM7_Init(void)
{
	TIM7_Handler.Instance=TIM7;             //������ʱ��6
	TIM7_Handler.Init.Prescaler=840-1;      //��Ƶϵ��
	TIM7_Handler.Init.Period=200-1;         //�Զ�װ��ֵ
	HAL_TIM_Base_Init(&TIM7_Handler);
    
	HAL_TIM_Base_Start_IT(&TIM7_Handler); //ʹ�ܶ�ʱ��7�Ͷ�ʱ��7�����жϣ�TIM_IT_UPDATE
}


void Input_Encoder_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOF_CLK_ENABLE();               //����GPIOFʱ��
	__HAL_RCC_GPIOG_CLK_ENABLE();               //����GPIOGʱ��
	
	//��ʼ����ת������A����������
	GPIO_InitStruct.Pin=BTN_A_Pin;                			
	GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;      //�½��ش����ⲿ�ж�
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(BTN_A_PIN_Port,&GPIO_InitStruct);
	
	//��ʼ����ת������B����������
	GPIO_InitStruct.Pin=BTN_B_Pin;                			
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;                  //��������ģʽ
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(BTN_B_PIN_Port,&GPIO_InitStruct);
	
	//��ʼ����������������
	GPIO_InitStruct.Pin = BEEPE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BEEPE_PIN_Port, &GPIO_InitStruct);	
	
	//�����ж���15_10
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //ʹ���ж���	
	
	//��ʼ���������Ķ�ʱ��7
	TIM7_Init();
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM7)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		__HAL_RCC_GPIOA_CLK_ENABLE();           //��������������GPIOAʱ��
		__HAL_RCC_TIM7_CLK_ENABLE();            //ʹ��TIM7ʱ��
		
		//��ʼ����ת����������
		GPIO_InitStruct.Pin=BTN_ENC_Pin;                			
		GPIO_InitStruct.Mode=GPIO_MODE_INPUT;                  //��������ģʽ
		GPIO_InitStruct.Pull=GPIO_PULLUP;
		HAL_GPIO_Init(BTN_ENC_PIN_Port,&GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(TIM7_IRQn,2,1);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //����ITM3�ж�   
	}
}

//��ʱ��7�жϷ�����
void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM7_Handler);       //���ö�ʱ���жϴ����ú���
}

//�ⲿ�ж�10-15�жϷ�����
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);		//�����ⲿ�жϴ����ú���
}

//��ʱ���ص���������ʱ���жϷ���������
//�������´������Ͱ�����������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM7_Handler))
	{
		if(Encoder_KEY != 0){
			Encoder_KEY_nu = 0;        //����������������ļ���
			Encoder_KEY_flage = 1;     //��ǰ������ͷ�
		}
		else{
			Encoder_KEY_nu++;
			//��������8�μ����������£�����һ��ȷ�ϰ����ѱ��ͷ�
			if(Encoder_KEY_nu>=8 && Encoder_KEY_flage)
			{
				Encoder_KEY_flage = 0;     //��ǰ���������
				switch(menu_show)
				{
					//���˵����ѡ��ȷ�Ͽ���
					case 0:
						switch(menu_current)
						{
							case 0:
								menu_show = 1;      //��ʾ���ƽ���˵�
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							break;
							case 1:
								menu_show = 2;      //ϵͳ���ÿ��ƽ���˵�
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							break;
							case 2:
								menu_show = 3;      //SD���˵�ѡ�����
								//д��SD����Ŀ¼·��
								strcpy(SD_path, "0:");
								//��ȡ��Ŀ¼���ļ��б�
								mf_get_file_name((uint8_t *)SD_path);
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							break;
						}
					break;
						
					//���ƽ�����ѡ��ȷ�Ͽ���
					case 1:
						switch(menu_current)
						{
							case 0:
								menu_show = 0;      //���ؿ��ƽ���˵�
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							break;
						
						}	
					break;
					
					//ϵͳ���ý�����ѡ��ȷ�Ͽ���
					case 2:
						switch(menu_current)
						{
							case 0:/*������һ��*/
								menu_show = 0;      //���ؿ��ƽ���˵�
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							break;
						
						}
					
					//SD��������ѡ��ȷ�Ͽ���
					case 3:
						//�ͷ�������ڴ�
						for(u8 i=0; i<SD_file_un; i++)
						{
							myfree(SD_item[i]);
						}
						
						//����SD���ļ���һ��·��
						if(menu_current == 0)
						{
							char dest[100] = {0};
							//����Ҫ��ȡ��һ��·����λ��
							char *a = strrchr(SD_path, '/');
							if(a)
							{
								//����Ҫ��ȡ�����ݳ���
								uint8_t date = strlen(SD_path) - strlen(a);
								//��ȡ��һ���ļ�·��
								memmove(dest, SD_path, date);
								//��·�����ǵ�֮ǰ·����
								strcpy(SD_path, dest);
								mf_get_file_name((uint8_t*)SD_path);
								printf("������һ���ļ��У�\r\n");
								printf("�ļ�Ŀ¼Ϊ��%s\r\n", SD_path);
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							}
							else
							{
								menu_show = 0;      //���ؿ��ƽ���˵�
								printf("�����ܲ˵���\r\n");
								menu_current = 0;		//���ù��Ϊ��ʼλ��
							}
						}
						//��ǰĿ¼�µ��ļ�ѡ��
						else
						{
							FRESULT res;
							//�ڸ�Ŀ¼�²��Ҷ�Ӧ���ļ�
							res = f_findfirst(&dir, &fileinfo, SD_path, SD_item[menu_current]);
							if(res == FR_OK && fileinfo.fname[0])
							{
								//�б���ļ��Ƿ�Ϊһ���ļ���
								if(fileinfo.fattrib&AM_DIR)
								{
									//�ļ�Ŀ¼ƴ�Ӵ���
									strcat(SD_path, "/");
									strcat(SD_path, SD_item[menu_current]);
									
									//��ȡ�µ�Ŀ¼�µ��ļ��б�
									mf_get_file_name((uint8_t*)SD_path);
									printf("��ѡ����һ���ļ��У�\r\n");
									printf("�ļ�Ŀ¼Ϊ��%s\r\n", SD_path);
									menu_current = 0;		//���ù��Ϊ��ʼλ��
								}
								else
								{
									//ѡ�����Ŀ���ļ�
									printf("��ѡ����һ���ļ���\r\n");
									printf("�ļ���Ϊ��%s\r\n", fileinfo.fname);
								}
							}
							f_closedir(&dir);
						}
					break;
					
				}	
				Beep_IsRun = 1;
				
				//����LCD12864��ʾ�˵�
				menu_update_flage = 1;
			}
		}
		
		//�õ�������������Ӧ����
		if(Beep_IsRun == 1)
		{
			//����������
			if(Beep_Run_falge == 0)
			{
				BEEP_ON;
				Beep_Run_falge = 1;
			}
			//��¼���������еĵ�λʱ�����
			Beep_RunTimeNum++;
			
			//���������Ԥ��ʱ�����
			//�رշ��������������������־λ
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


//�ⲿ�жϷ������
//��ת���뿪�ش�����
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		//��ת��������ť��ת��
		case GPIO_PIN_10:
			//��һ���жϣ�����A�����½��أ�������������
			if(Encoder_A==0 && Encoder_AB_nu==0)
			{
				Encoder_AB_flage=0;
				if(Encoder_B == 1)     //����B�࣬�趨��־
					Encoder_AB_flage=1;
				Encoder_AB_nu=1;
			}
			
			//�ڶ����жϣ�����A���������أ�ȷ�ϱ�����ת�������жϷ���
			if(Encoder_A==1 && Encoder_AB_nu==1)
			{
				//��������ʱ��ת��
				if(Encoder_B==0 && Encoder_AB_flage==1)
				{
					switch(menu_show)
					{
						//���˵���������ƶ�����
						case 0:   //������˵�
							menu_current--;
							if(menu_current<0)
								menu_current = menu_main-1;
						break;
							
						case 1:  //���ƽ���˵�
							menu_current--;
							if(menu_current<0)
								menu_current = menu_control-1;
						break;
							
						case 2:  //ϵͳ���ý���˵�
							menu_current--;
							if(menu_current<0)
								menu_current = menu_setting-1;
						break;
						
						case 3:  //SD������˵�
							menu_current--;
							if(menu_current<0)
								menu_current = SD_file_un-1;
						break;
					}
					
					//����LCD12864��ʾ�˵�
					menu_update_flage = 1;
				}
				
				//������˳ʱ��ת��
				if(Encoder_B==1 && Encoder_AB_flage==0)
				{
					switch(menu_show)
					{
						//���˵���������ƶ�����
						case 0:                     //���˵�
							menu_current++;
							if(menu_current>=menu_main)
								menu_current = 0;
						break;
							
						case 1:  //���ƽ���˵�
							menu_current++;
							if(menu_current>=menu_control)
								menu_current = 0;
						break;
						
						case 2:  //ϵͳ���ý���˵�
							menu_current++;
							if(menu_current>=menu_setting)
								menu_current = 0;
						break;
						
						case 3:  //SD������˵�
							menu_current++;
							if(menu_current>=SD_file_un)
								menu_current = 0;
						break;							
					}
					
					//����LCD12864��ʾ�˵�
					menu_update_flage = 1;
				}
				Encoder_AB_nu = 0;
			}
		break;
	}
}


