#ifndef __M_MISC_H
#define __M_MISC_H

#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "m_Serial.h"

#define PROGMEM
//#define __INLINE inline

#define millis(x)         HAL_GetTick(x)

#define PIN_SET(GPIO_Port, GPIO_Pin)       HAL_GPIO_WritePin(GPIO_Port, GPIO_Pin, GPIO_PIN_SET)
#define PIN_RESET(GPIO_Port, GPIO_Pin)     HAL_GPIO_WritePin(GPIO_Port, GPIO_Pin, GPIO_PIN_RESET)
#define PIN_TOGGLE(GPIO_Port, GPIO_Pin)    HAL_GPIO_TogglePin(GPIO_Port, GPIO_Pin)

void m_printPGM(const char *str);
void m_serialprintPGM(const char *str);
void m_serialprintlong(long c);

#endif
