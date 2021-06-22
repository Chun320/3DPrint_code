#ifndef __INPUT_KEY_H
#define __INPUT_KEY_H

#include "sys.h"

#define Encoder_KEY   PAin(8)
#define Encoder_A     PGin(10)
#define Encoder_B     PFin(11)
#define BEEP_ON       PGout(4) = 1
#define BEEP_OFF      PGout(4) = 0

extern uint8_t menu_show;         //��ʾ���˵������ַ�Ĵ���
extern uint8_t control_menu_show; //��ʾ���ƻ����ַ�Ĵ���

extern int8_t menu_current;       //���õ�ǰ�˵����λ��
extern uint8_t menu_update_flage; //��Ļˢ�±�־λ


extern u8 KEY_IsPress;
extern u8 Encoder_Data;

void Input_Encoder_Init(void);

#endif

