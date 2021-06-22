#ifndef __INPUT_KEY_H
#define __INPUT_KEY_H

#include "sys.h"

#define Encoder_KEY   PAin(8)
#define Encoder_A     PGin(10)
#define Encoder_B     PFin(11)
#define BEEP_ON       PGout(4) = 1
#define BEEP_OFF      PGout(4) = 0

extern uint8_t menu_show;         //显示主菜单画面地址寄存器
extern uint8_t control_menu_show; //显示控制画面地址寄存器

extern int8_t menu_current;       //设置当前菜单光标位置
extern uint8_t menu_update_flage; //屏幕刷新标志位


extern u8 KEY_IsPress;
extern u8 Encoder_Data;

void Input_Encoder_Init(void);

#endif

