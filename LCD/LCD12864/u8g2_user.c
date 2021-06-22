#include "u8g2_user.h"
#include "m_PinGpio.h"
#include "input_key.h"
#include "fattester.h"
#include "delay.h"
#include "string.h"
#include "ff.h"

#include "m_Serial.h"

u8g2_t u8g2;


//主菜单目录
const char *Main_Screen[] = {"Control Panel", "System Setting", "Print Form SD", "Print Form USB", "Power OFF"};
//控制菜单目录
const char *Control[] = {"Back", "Auto Home", "Move Axis", "Disable steppers", "Fan Control", "LED Control"};
//系统设置菜单目录
const char *Setting[] = {"Back", "Axis Steps", "Axsi Jerk", "Axis Vmax", "Axis Amax", "Acceleration", "Retract_Accel"};

uint8_t menu_main = sizeof(Main_Screen)/sizeof(Main_Screen[0]);        //获取主菜单个数
uint8_t menu_control = sizeof(Control)/sizeof(Control[0]);             //获取控制菜单个数
uint8_t menu_setting = sizeof(Setting)/sizeof(Setting[0]);             //获取控制菜单个数



uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//初始化引脚
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			LCD12864_Pin_Init();
		break;

		//毫秒级延时
		case U8X8_MSG_DELAY_MILLI:
			delay_ms(arg_int);
		break;
		
		//延时10us
		case U8X8_MSG_DELAY_10MICRO:
			delay_us(10);
		break;
		
		//延时100ns
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

/*LCD中文显示程序
 *u8g2:u8g2显示句柄
 *   x:显示到显示器中的X轴坐标
 *   x:显示到显示器中的Y轴坐标
 * str:需要在显示器显示的中文字符串
 *  例: wu8g2_DrawStr(&u8g2, 0, 14, "移动电机AaBb号");
*/
void wu8g2_DrawStr(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, char *str)
{
	u8 string_len;
	u16 wString;
	WCHAR result;
	//获取中文字符长度，中文占两个字符
	string_len = strlen(str);
	//针对每一个中文字符进行处理
	for(u8 i=0; i<string_len; i++)
	{
		wString = 0;
		//如果字符为英文
		if((u8)str[i] < 128)
		{
			//直接通过字符串函数输出
			u8g2_DrawStr(u8g2, x, y, &str[i]);
			x+=8;
		}
		//如果字符为中文
		else
		{
			//将两个字符拼接为一个中文字符；得到GBK编码字符
			wString |= str[i]<<8; //获取GBK编码的高8位字节
			i++;                  
			wString |= str[i]; //获取GBK编码的低8位字节
			
			//将得到的GBK编码字符转换为unicode编码字符
			result = ff_convert(wString, 1);
			//同过unicode编码，将字符显示到显示中
			u8g2_DrawGlyph(u8g2, x, y, result);
			//显示位置移到下一个字符位置
			x+=16;
		}
	}
}

void LCD12864_Init(void)
{
	u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, u8g2_gpio_and_delay_stm32);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
	
	//LCD初始化完成后，开始显示菜单
	menu_update_flage = 1;
}

void LCD12864_UpDate(void)
{
	uint8_t i, h;
  u8g2_uint_t w, d;
	u8 menu_offset = 0;                 //显示菜单偏移量
	u8 menu_free = 0;
	
	u8g2_ClearDisplay(&u8g2);           //清除上一次LCD屏幕
  u8g2_SetFont(&u8g2, u8g_font_7x14); //设置显示文字字体
	u8g2_SetFontPosTop(&u8g2);          //设置显示光标置顶
  //获取字体高度
	h = u8g2_GetFontAscent(&u8g2)-u8g2_GetFontDescent(&u8g2)+1;
  //获取显示器宽度
	w = u8g2_GetDisplayWidth(&u8g2);
	
	//通过当前菜单光标位置，计算显示菜单偏移量
	if(menu_current-LCD_LINE_NUM > -1)
	{
		menu_offset = menu_current-LCD_LINE_NUM+1;
	}
	//当SD卡当前菜单数小于LCD_LINE_NUM(LCD显示菜单数量)时；计算空余菜单数
	if(LCD_LINE_NUM-SD_file_un > 0)
	{
		menu_free = LCD_LINE_NUM-SD_file_un;
	}
	
	switch(menu_show)
	{
		//主界面菜单显示
		case 0:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//计算出字符的中间宽度位置；使字符居中显示
				d = (w-u8g2_GetStrWidth(&u8g2, Main_Screen[i+menu_offset]))/2;

        //设置绘制颜色为实体颜色
        u8g2_SetDrawColor(&u8g2, 1);

        //判别当前菜单是否被选中；若被选中则显示为背景色
        if(i+menu_offset == menu_current)
        {
          //绘制一个矩形实心
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //设置绘制颜色为透明颜色
          u8g2_SetDrawColor(&u8g2, 0);
        }
				//将对应的字符显示到屏幕中
				u8g2_DrawStr(&u8g2, d, i*h, Main_Screen[i+menu_offset]);
      }
			break;
			
		//控制界面菜单显示
		case 1:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//计算出字符的中间宽度位置；使字符居中显示
				d = (w-u8g2_GetStrWidth(&u8g2, Control[i+menu_offset]))/2;

        //设置绘制颜色为实体颜色
        u8g2_SetDrawColor(&u8g2, 1);

        //判别当前菜单是否被选中；若被选中则显示为背景色
        if(i+menu_offset == menu_current)
        {
          //绘制一个矩形实心
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //设置绘制颜色为透明颜色
          u8g2_SetDrawColor(&u8g2, 0);
        }
          //将对应的字符显示到屏幕中
          u8g2_DrawStr(&u8g2, d, i*h, Control[i+menu_offset]);
      }
		break;
		
		//控制界面菜单显示
		case 2:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset; i++)
      {
				//计算出字符的中间宽度位置；使字符居中显示
				d = (w-u8g2_GetStrWidth(&u8g2, Setting[i+menu_offset]))/2;

        //设置绘制颜色为实体颜色
        u8g2_SetDrawColor(&u8g2, 1);

        //判别当前菜单是否被选中；若被选中则显示为背景色
        if(i+menu_offset == menu_current)
        {
          //绘制一个矩形实心
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //设置绘制颜色为透明颜色
          u8g2_SetDrawColor(&u8g2, 0);
        }
          //将对应的字符显示到屏幕中
          u8g2_DrawStr(&u8g2, d, i*h, Setting[i+menu_offset]);
      }
			break;
			
			//SD卡菜单选择界面显示
		case 3:
			for(i = 0; i+menu_offset < LCD_LINE_NUM+menu_offset-menu_free; i++)
      {
				//计算出字符的中间宽度位置；使字符居中显示
				d = (w-u8g2_GetStrWidth(&u8g2, SD_item[i+menu_offset]))/2;

        //设置绘制颜色为实体颜色
        u8g2_SetDrawColor(&u8g2, 1);

        //判别当前菜单是否被选中；若被选中则显示为背景色
        if(i+menu_offset == menu_current)
        {
          //绘制一个矩形实心
          u8g2_DrawBox(&u8g2, 0, i*h, w, h);
          //设置绘制颜色为透明颜色
          u8g2_SetDrawColor(&u8g2, 0);
        }
				if(i==0 && menu_offset==0)
					//将对应的字符显示到屏幕中（居中显示）
					u8g2_DrawStr(&u8g2, d, i*h, SD_item[i+menu_offset]);
				else
					//将对应的字符显示到屏幕中（靠左显示）
					u8g2_DrawStr(&u8g2, 1, i*h, SD_item[i+menu_offset]);
      }
			break;
	
	}
	u8g2_SendBuffer(&u8g2);
	menu_update_flage = 0;
}
	
