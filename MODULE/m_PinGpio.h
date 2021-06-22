#ifndef __M_PINGPIO_H
#define __M_PINGPIO_H

#include "sys.h"
#include "m_pin.h"

#define RUN_LED  PAout(7)    //�忨����ָʾ��

/***************************�忨ָʾ������ʱ�ӿ���***************************/
#define ENABLE_RUN_LED_RCC_CLOCK        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISABLE_RUN_LED_RCC_CLOCK       __HAL_RCC_GPIOA_CLK_DISABLE()

/***************************X����λ����ʱ�ӿ���***************************/
#define ENABLE_X_MIN_PIN_RCC_CLOCK      __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISABLE_X_MIN_PIN_RCC_CLOCK     __HAL_RCC_GPIOB_CLK_DISABLE()
#define ENABLE_X_MAX_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_X_MAX_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()

/***************************Y����λ����ʱ�ӿ���***************************/
#define ENABLE_Y_MIN_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_Y_MIN_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_Y_MAX_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_Y_MAX_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()

/***************************Z����λ����ʱ�ӿ���***************************/
#define ENABLE_Z_MIN_PIN_RCC_CLOCK      __HAL_RCC_GPIOG_CLK_ENABLE()
#define DISABLE_Z_MIN_PIN_RCC_CLOCK     __HAL_RCC_GPIOG_CLK_DISABLE()
#define ENABLE_Z_MAX_PIN_RCC_CLOCK      __HAL_RCC_GPIOG_CLK_ENABLE()
#define DISABLE_Z_MAX_PIN_RCC_CLOCK     __HAL_RCC_GPIOG_CLK_DISABLE()

/***************************X������������ʱ�ӿ���***************************/
#define ENABLE_X_STEP_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_X_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_X_DIR_PIN_RCC_CLOCK       __HAL_RCC_GPIOF_CLK_ENABLE()
#define DISABLE_X_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOF_CLK_DISABLE()
#define ENABLE_X_ENABLE_PIN_RCC_CLOCK    __HAL_RCC_GPIOF_CLK_ENABLE()
#define DISABLE_X_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOF_CLK_DISABLE()

/***************************Y������������ʱ�ӿ���***************************/
#define ENABLE_Y_STEP_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_Y_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_Y_DIR_PIN_RCC_CLOCK       __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_Y_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_Y_ENABLE_PIN_RCC_CLOCK    __HAL_RCC_GPIOD_CLK_ENABLE()
#define DISABLE_Y_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOD_CLK_DISABLE()

/***************************Z������������ʱ�ӿ���***************************/
#define ENABLE_Z_STEP_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_Z_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_Z_DIR_PIN_RCC_CLOCK       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DISABLE_Z_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOC_CLK_DISABLE()
#define ENABLE_Z_ENABLE_PIN_RCC_CLOCK    __HAL_RCC_GPIOC_CLK_ENABLE()
#define DISABLE_Z_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOC_CLK_DISABLE()

/***************************E0������������ʱ�ӿ���***************************/
#define ENABLE_E0_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_E0_STEP_PIN_RCC_CLOCK    __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_E0_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISABLE_E0_DIR_PIN_RCC_CLOCK     __HAL_RCC_GPIOA_CLK_DISABLE()
#define ENABLE_E0_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOC_CLK_ENABLE()
#define DISABLE_E0_ENABLE_PIN_RCC_CLOCK  __HAL_RCC_GPIOC_CLK_DISABLE()

/***************************E1������������ʱ�ӿ���***************************/
#define ENABLE_E1_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOD_CLK_ENABLE()
#define DISABLE_E1_STEP_PIN_RCC_CLOCK    __HAL_RCC_GPIOD_CLK_DISABLE()
#define ENABLE_E1_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DISABLE_E1_DIR_PIN_RCC_CLOCK     __HAL_RCC_GPIOE_CLK_DISABLE()
#define ENABLE_E1_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISABLE_E1_ENABLE_PIN_RCC_CLOCK  __HAL_RCC_GPIOA_CLK_DISABLE()

/***************************E2������������ʱ�ӿ���***************************/
#define ENABLE_E2_STEP_PIN_RCC_CLOCK     __HAL_RCC_GPIOD_CLK_ENABLE()
#define DISABLE_E2_STEP_PIN_RCC_CLOCK    __HAL_RCC_GPIOD_CLK_DISABLE()
#define ENABLE_E2_DIR_PIN_RCC_CLOCK      __HAL_RCC_GPIOG_CLK_ENABLE()
#define DISABLE_E2_DIR_PIN_RCC_CLOCK     __HAL_RCC_GPIOG_CLK_DISABLE()
#define ENABLE_E2_ENABLE_PIN_RCC_CLOCK   __HAL_RCC_GPIOF_CLK_ENABLE()
#define DISABLE_E2_ENABLE_PIN_RCC_CLOCK  __HAL_RCC_GPIOF_CLK_DISABLE()


void m_LED_Init(void);
void LCD12864_Pin_Init(void);

#endif


