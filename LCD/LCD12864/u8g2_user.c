#include "u8g2_user.h"
#include "m_PinGpio.h"
#include "input_key.h"
#include "fattester.h"
#include "delay.h"
#include "string.h"
#include "ff.h"

#include "m_Serial.h"

u8g2_t u8g2;


//���˵�Ŀ¼
const char *Main_Screen[] = {"Control Panel", "System Setting", "Print Form SD", "Print Form USB", "Power OFF"};
//���Ʋ˵�Ŀ¼
const char *Control[] = {"Back", "Auto Home", "Move Axis", "Disable steppers", "Fan Control", "LED Control"};
//ϵͳ���ò˵�Ŀ¼
const char *Setting[] = {"Back", "Axis Steps", "Axsi Jerk", "Axis Vmax", "Axis Amax", "Acceleration", "Retract_Accel"};

uint8_t menu_main = sizeof(Main_Screen)/sizeof(Main_Screen[0]);        //��ȡ���˵�����
uint8_t menu_control = sizeof(Control)/sizeof(Control[0]);             //��ȡ���Ʋ˵�����
uint8_t menu_setting = sizeof(Setting)/sizeof(Setting[0]);             //��ȡ���Ʋ˵�����



uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//��ʼ������
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			LCD12864_Pin_Init();
		break;

		//���뼶��ʱ
		case U8X8_MSG_DELAY_MILLI:
			delay_ms(arg_int);
		break;
		
		//��ʱ10us
		case U8X8_MSG_DELAY_10MICRO:
			delay_us(10);
		break;
		
		//��ʱ100ns
		case U8X8_MSG_DELAY_100NANO:
			__NOP();
			__NOP();
		break;
		
		case U8X8_MSG_GPIO_SPI_CLOCK:
			if (arg_int) LCD_SCLK=1;
			else LCD_SCLK=0;
		break;
		
		case U8X8_MSG_GPIO_SPI_DATA:
			if (arg_int) LCD_SID=1;
			else LCD_SID=0;
		break;
		
		case U8X8_MSG_GPIO_CS:
			if (arg_int) LCD_RS=1	;
			else LCD_RS=0;
		break;
		
		default:
			return 0;
	}
	return 1;
}

/*LCD������ʾ����
 *u8g2:u8g2��ʾ���
 *   x:��ʾ����ʾ���е�X������
 *   x:��ʾ����ʾ���е�Y������
 * str:��Ҫ����ʾ����ʾ�������ַ���
 *  ��: wu8g2_DrawStr(&u8g2, 0, 14, "�ƶ����AaBb��");
*/
void wu8g2_DrawStr(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, char *str)
{
	u8 string_len;
	u16 wString;
	WCHAR result;
	//��ȡ�����ַ����ȣ�����ռ�����ַ�
	string_len = strlen(str);
	//���ÿһ�������ַ����д���
	for(u8 i=0; i<string_len; i++)
	{
		wString = 0;
		//����ַ�ΪӢ��
		if((u8)str[i] < 128)
		{
			//ֱ��ͨ���ַ����������
			u8g2_DrawStr(u8g2, x, y, &str[i]);
			x+=8;
		}
		//����ַ�Ϊ����
		else
		{
			//�������ַ�ƴ��Ϊһ�������ַ����õ�GBK�����ַ�
			wString |= str[i]<<8; //��ȡGBK����ĸ�8λ�ֽ�
			i++;                  
			wString |= str[i]; //��ȡGBK����ĵ�8λ�ֽ�
			
			//���õ���GBK�����ַ�ת��Ϊunicode�����ַ�
			result = ff_convert(wString, 1);
			//ͬ��unicode���룬���ַ���ʾ����ʾ��
			u8g2_DrawGlyph(u8g2, x, y, result);
			//��ʾλ���Ƶ���һ���ַ�λ��
			x+=16;
		}
	}
}

void LCD12864_Init(void)
{
	u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, u8g2_gpio_and_delay_stm32);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
	
	//LCD��ʼ����ɺ󣬿�ʼ��ʾ�˵�
	menu_update_flage = 1;
}

void LCD12864_UpDate(void)
{
	uint8_t i, h;
  u8g2_uint_t w, d;
	u8 menu_offset = 0;                 //��ʾ�˵�ƫ����
	u8 menu_free = 0;
	
	u8g2_ClearDisplay(&u8g2);           //�����һ��LCD��Ļ
  u8g2_SetFont(&u8g2, u8g_font_7x14); //������ʾ��������
	u8g2_SetFontPosTop(&u8g2);          //������ʾ����ö�
  //��ȡ����߶�
	h = u8g2_GetFontAscent(&u8g2)-u8g2_GetFontDescent(&u8g2)+1;
  //��ȡ��ʾ�����
	w = u8g2_GetDisplayWidth(&u8g2);
	
	//ͨ����ǰ�˵����λ�ã�������ʾ�˵�ƫ����
	if(menu_current-LCD_LINE_NUM > -1)
	{
		menu_offset = menu_current-LCD_LINE_NUM+1;
	}
	//��SD����ǰ�˵���С��LCD_LINE_NUM(LCD��ʾ�˵�����)ʱ���������˵���
	if(LCD_LINE_NUM-SD_file_un > 0)
	{
		menu_free = LCD_LINE_NUM-SD_file_un;
	}
	
	switch(menu_show)
	{
		//������˵���ʾ
		case 0:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//������ַ����м���λ�ã�ʹ�ַ�������ʾ
				d = (w-u8g2_GetStrWidth(&u8g2, Main_Screen[i+menu_offset]))/2;

        //���û�����ɫΪʵ����ɫ
        u8g2_SetDrawColor(&u8g2, 1);

        //�б�ǰ�˵��Ƿ�ѡ�У�����ѡ������ʾΪ����ɫ
        if(i+menu_offset == menu_current)
        {
          //����һ������ʵ��
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //���û�����ɫΪ͸����ɫ
          u8g2_SetDrawColor(&u8g2, 0);
        }
				//����Ӧ���ַ���ʾ����Ļ��
				u8g2_DrawStr(&u8g2, d, i*h, Main_Screen[i+menu_offset]);
      }
			break;
			
		//���ƽ���˵���ʾ
		case 1:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//������ַ����м���λ�ã�ʹ�ַ�������ʾ
				d = (w-u8g2_GetStrWidth(&u8g2, Control[i+menu_offset]))/2;

        //���û�����ɫΪʵ����ɫ
        u8g2_SetDrawColor(&u8g2, 1);

        //�б�ǰ�˵��Ƿ�ѡ�У�����ѡ������ʾΪ����ɫ
        if(i+menu_offset == menu_current)
        {
          //����һ������ʵ��
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //���û�����ɫΪ͸����ɫ
          u8g2_SetDrawColor(&u8g2, 0);
        }
          //����Ӧ���ַ���ʾ����Ļ��
          u8g2_DrawStr(&u8g2, d, i*h, Control[i+menu_offset]);
      }
		break;
		
		//���ƽ���˵���ʾ
		case 2:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//������ַ����м���λ�ã�ʹ�ַ�������ʾ
				d = (w-u8g2_GetStrWidth(&u8g2, Setting[i+menu_offset]))/2;

        //���û�����ɫΪʵ����ɫ
        u8g2_SetDrawColor(&u8g2, 1);

        //�б�ǰ�˵��Ƿ�ѡ�У�����ѡ������ʾΪ����ɫ
        if(i+menu_offset == menu_current)
        {
          //����һ������ʵ��
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //���û�����ɫΪ͸����ɫ
          u8g2_SetDrawColor(&u8g2, 0);
        }
          //����Ӧ���ַ���ʾ����Ļ��
          u8g2_DrawStr(&u8g2, d, i*h, Setting[i+menu_offset]);
      }
			break;
			
			//SD���˵�ѡ�������ʾ
		case 3:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset-menu_free; i++)
      {
				//������ַ����м���λ�ã�ʹ�ַ�������ʾ
				d = (w-u8g2_GetStrWidth(&u8g2, SD_item[i+menu_offset]))/2;

        //���û�����ɫΪʵ����ɫ
        u8g2_SetDrawColor(&u8g2, 1);

        //�б�ǰ�˵��Ƿ�ѡ�У�����ѡ������ʾΪ����ɫ
        if(i+menu_offset == menu_current)
        {
          //����һ������ʵ��
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //���û�����ɫΪ͸����ɫ
          u8g2_SetDrawColor(&u8g2, 0);
        }
				if(i==0 && menu_offset==0)
					//����Ӧ���ַ���ʾ����Ļ�У�������ʾ��
					u8g2_DrawStr(&u8g2, d, i*h, SD_item[i+menu_offset]);
				else
					//����Ӧ���ַ���ʾ����Ļ�У�������ʾ��
					u8g2_DrawStr(&u8g2, 1, i*h, SD_item[i+menu_offset]);
      }
			break;
	
	}
	u8g2_SendBuffer(&u8g2);
	menu_update_flage = 0;
}
	
