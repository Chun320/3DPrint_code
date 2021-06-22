#ifndef __M_SPI_H
#define __M_SPI_H

#include "stm32f4xx_hal.h"
#include "m_pin.h"

extern SPI_HandleTypeDef SPI1_Handler;  //SPI¾ä±ú

void SPI1_Init(void);
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);

#endif

