#ifndef __U8G2_USER_H
#define __U8G2_USER_H

#include "stdio.h"
#include "u8g2.h"

#define LCD_LINE_NUM     5        //LCD12864显示菜单行数

#define LCD_RS	         PDout(10)  //LCD12864片选引脚
#define LCD_SCLK         PGout(2)
#define LCD_SID          PDout(11)

extern uint8_t menu_main;
extern uint8_t menu_control;
extern uint8_t menu_setting;

extern u8g2_t u8g2;

void LCD12864_Init(void);
void LCD12864_UpDate(void);

#endif

