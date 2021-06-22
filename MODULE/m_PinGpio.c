#include "m_PinGpio.h"

/***************************初始化运行指示灯***************************/
void m_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ENABLE_RUN_LED_RCC_CLOCK;
    
    GPIO_InitStruct.Pin = RUN_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RUN_LED_PIN_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(RUN_LED_PIN_Port, RUN_LED_Pin, GPIO_PIN_SET);
}

/***************************初始化打印机限位引脚***************************/
void m_EndStop_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t gpioPin;
    GPIO_TypeDef* gpioPort;
    for ( uint8_t i = 0; i < 6; i++)
    {
        switch (i)
        {
        case 0:
            ENABLE_X_MIN_PIN_RCC_CLOCK;
            gpioPin = X_MIN_Pin;
            gpioPort = X_MIN_PIN_Port;
            break;
        case 1:
            ENABLE_X_MAX_PIN_RCC_CLOCK;
            gpioPin = X_MAX_Pin;
            gpioPort = X_MAX_PIN_Port;
            break;
        case 2:
            ENABLE_Y_MIN_PIN_RCC_CLOCK;
            gpioPin = Y_MIN_Pin;
            gpioPort = Y_MIN_PIN_Port;
            break;
        case 3:
            ENABLE_Y_MAX_PIN_RCC_CLOCK;
            gpioPin = Y_MAX_Pin;
            gpioPort = Y_MAX_PIN_Port;
            break;
        case 4:
            ENABLE_Z_MIN_PIN_RCC_CLOCK;
            gpioPin = Z_MIN_Pin;
            gpioPort = Z_MIN_PIN_Port;
            break;
        case 5:
            ENABLE_Z_MAX_PIN_RCC_CLOCK;
            gpioPin = Z_MAX_Pin;
            gpioPort = Z_MAX_PIN_Port;
            break;
        default:
            break;
        }
        GPIO_InitStruct.Pin = gpioPin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
    }
    
}

/***************************初始化打印机电机控制引脚***************************/
void m_Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t gpioPin;
    GPIO_TypeDef* gpioPort;
    for(uint8_t i = 0; i < 6; i++)
    {
        for(uint8_t a = 0; a < 3; a++)
        {
            switch (i)
            {
            case 0:
                switch (a)
                {
                case 0:
                    ENABLE_X_STEP_PIN_RCC_CLOCK;
                    gpioPin = X_STEP_Pin;
                    gpioPort = X_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_X_DIR_PIN_RCC_CLOCK;
                    gpioPin = X_DIR_Pin;
                    gpioPort = X_DIR_PIN_Port;
                case 2:
                    ENABLE_X_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = X_ENABLE_Pin;
                    gpioPort = X_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            case 1:
                switch (a)
                {
                case 0:
                    ENABLE_Y_STEP_PIN_RCC_CLOCK;
                    gpioPin = Y_STEP_Pin;
                    gpioPort = Y_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_Y_DIR_PIN_RCC_CLOCK;
                    gpioPin = Y_DIR_Pin;
                    gpioPort = Y_DIR_PIN_Port;
                case 2:
                    ENABLE_Y_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = Y_ENABLE_Pin;
                    gpioPort = Y_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            case 2:
                switch (a)
                {
                case 0:
                    ENABLE_Z_STEP_PIN_RCC_CLOCK;
                    gpioPin = Z_STEP_Pin;
                    gpioPort = Z_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_Z_DIR_PIN_RCC_CLOCK;
                    gpioPin = Z_DIR_Pin;
                    gpioPort = Z_DIR_PIN_Port;
                case 2:
                    ENABLE_Z_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = Z_ENABLE_Pin;
                    gpioPort = Z_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            case 3:
                switch (a)
                {
                case 0:
                    ENABLE_E0_STEP_PIN_RCC_CLOCK;
                    gpioPin = E0_STEP_Pin;
                    gpioPort = E0_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_E0_DIR_PIN_RCC_CLOCK;
                    gpioPin = E0_DIR_Pin;
                    gpioPort = E0_DIR_PIN_Port;
                case 2:
                    ENABLE_E0_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = E0_ENABLE_Pin;
                    gpioPort = E0_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            case 4:
                switch (a)
                {
                case 0:
                    ENABLE_E1_STEP_PIN_RCC_CLOCK;
                    gpioPin = E1_STEP_Pin;
                    gpioPort = E1_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_E1_DIR_PIN_RCC_CLOCK;
                    gpioPin = E1_DIR_Pin;
                    gpioPort = E1_DIR_PIN_Port;
                case 2:
                    ENABLE_E1_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = E1_ENABLE_Pin;
                    gpioPort = E1_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            case 5:
                switch (a)
                {
                case 0:
                    ENABLE_E2_STEP_PIN_RCC_CLOCK;
                    gpioPin = E2_STEP_Pin;
                    gpioPort = E2_STEP_PIN_Port;
                    break;
                case 1:
                    ENABLE_E2_DIR_PIN_RCC_CLOCK;
                    gpioPin = E2_DIR_Pin;
                    gpioPort = E2_DIR_PIN_Port;
                case 2:
                    ENABLE_E2_ENABLE_PIN_RCC_CLOCK;
                    gpioPin = E2_ENABLE_Pin;
                    gpioPort = E2_ENABLE_PIN_Port;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            GPIO_InitStruct.Pin = gpioPin;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
            HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);    
            HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);
        }
    }
}

//LCD12864引脚初始化 
void LCD12864_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* 开启LCD12864引脚时钟 */
  __HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	//初始化LCD12864引脚
	GPIO_InitStruct.Pin    = GPIO_PIN_11|GPIO_PIN_10;
	GPIO_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull   = GPIO_NOPULL;
	GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin    = GPIO_PIN_2|GPIO_PIN_6;
	GPIO_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull   = GPIO_NOPULL;
	GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
