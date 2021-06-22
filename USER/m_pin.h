#ifndef __M_PIN_H
#define __M_PIN_H

#include "config.h"

#define RUN_LED_PIN_Port                      GPIOA
#define RUN_LED_Pin                           GPIO_PIN_7

/*********************X轴电机限位引脚定义*******************/
//#define X_MIN_PIN                           PB10
#define X_MIN_PIN_Port                        GPIOB
#define X_MIN_Pin                             GPIO_PIN_10
//#define X_MAX_PIN                           PE15
#define X_MAX_PIN_Port                        GPIOE
#define X_MAX_Pin                             GPIO_PIN_15

/*********************Y轴电机限位引脚定义*******************/
//#define Y_MIN_PIN                           PE12 
#define Y_MIN_PIN_Port                        GPIOE
#define Y_MIN_Pin                             GPIO_PIN_12
//#define Y_MAX_PIN                           PE10 
#define Y_MAX_PIN_Port                        GPIOE
#define Y_MAX_Pin                             GPIO_PIN_10 

/*********************Z轴电机限位引脚定义*******************/
//#define Z_MIN_PIN                           PG8  
#define Z_MIN_PIN_Port                        GPIOG
#define Z_MIN_Pin                             GPIO_PIN_8 
//#define Z_MAX_PIN                           PG5
#define Z_MAX_PIN_Port                        GPIOG
#define Z_MAX_Pin                             GPIO_PIN_5

/*********************X轴电机控制引脚定义*******************/
//#define X_STEP_PIN                          PE9
#define X_STEP_PIN_Port                       GPIOE
#define X_STEP_Pin                            GPIO_PIN_9
//#define X_DIR_PIN                           PF1
#define X_DIR_PIN_Port                        GPIOF
#define X_DIR_Pin                             GPIO_PIN_1
//#define X_ENABLE_PIN                        PF2
#define X_ENABLE_PIN_Port                     GPIOF
#define X_ENABLE_Pin                          GPIO_PIN_2

/*********************Y轴电机控制引脚定义*******************/
//#define Y_STEP_PIN                          PE11
#define Y_STEP_PIN_Port                       GPIOE
#define Y_STEP_Pin                            GPIO_PIN_11
//#define Y_DIR_PIN                           PE8
#define Y_DIR_PIN_Port                        GPIOE
#define Y_DIR_Pin                             GPIO_PIN_8
//#define Y_ENABLE_PIN                        PD7
#define Y_ENABLE_PIN_Port                     GPIOD
#define Y_ENABLE_Pin                          GPIO_PIN_7

/*********************Z轴电机控制引脚定义*******************/
//#define Z_STEP_PIN                          PE13
#define Z_STEP_PIN_Port                       GPIOE
#define Z_STEP_Pin                            GPIO_PIN_13
//#define Z_DIR_PIN                           PC2
#define Z_DIR_PIN_Port                        GPIOC
#define Z_DIR_Pin                             GPIO_PIN_2
//#define Z_ENABLE_PIN                        PC0
#define Z_ENABLE_PIN_Port                     GPIOC
#define Z_ENABLE_Pin                          GPIO_PIN_0

/*********************E0轴电机控制引脚定义*******************/
//#define E0_STEP_PIN                         PE14
#define E0_STEP_PIN_Port                      GPIOE
#define E0_STEP_Pin                           GPIO_PIN_14
//#define E0_DIR_PIN                          PA0
#define E0_DIR_PIN_Port                       GPIOA
#define E0_DIR_Pin                            GPIO_PIN_0
//#define E0_ENABLE_PIN                       PC3
#define E0_ENABLE_PIN_Port                    GPIOC
#define E0_ENABLE_Pin                         GPIO_PIN_3

/*********************E1轴电机控制引脚定义*******************/
//#define E1_STEP_PIN                         PD15
#define E1_STEP_PIN_Port                      GPIOD
#define E1_STEP_Pin                           GPIO_PIN_15
//#define E1_DIR_PIN                          PE7
#define E1_DIR_PIN_Port                       GPIOE
#define E1_DIR_Pin                            GPIO_PIN_7
//#define E1_ENABLE_PIN                       PA3
#define E1_ENABLE_PIN_Port                    GPIOA
#define E1_ENABLE_Pin                         GPIO_PIN_3

/*********************E2轴电机控制引脚定义*******************/
//#define E2_STEP_PIN                         PD13
#define E2_STEP_PIN_Port                      GPIOD
#define E2_STEP_Pin                           GPIO_PIN_13
//#define E2_DIR_PIN                          PG9
#define E2_DIR_PIN_Port                       GPIOG
#define E2_DIR_Pin                            GPIO_PIN_9
//#define E2_ENABLE_PIN                       PF0
#define E2_ENABLE_PIN_Port                    GPIOF
#define E2_ENABLE_Pin                         GPIO_PIN_0


#ifdef ONBOARD_SD
    //#define SDSS                             PA4
    #define SDSS_PIN_Port                      GPIOA
    #define SDSS_Pin                           GPIO_PIN_4
    //#define SCK_PIN                          PA5
    #define SCK_PIN_Port                       GPIOA
    #define SCK_Pin                            GPIO_PIN_5
    //#define MISO_PIN                         PA6
    #define MISO_PIN_Port                      GPIOA
    #define MISO_Pin                           GPIO_PIN_6
    //#define MOSI_PIN                         PB5
    #define MOSI_PIN_Port                      GPIOB
    #define MOSI_Pin                           GPIO_PIN_5
    //#define SD_DETECT_PIN                    PB11
    #define SD_DETECT_PIN_Port                 GPIOB
    #define SD_DETECT_Pin                      GPIO_PIN_11
#endif


/*************************************LCD12864液晶模块引脚定义*******************************/
/**
 *               _____                                             _____
 *           NC | · · | GND                                    5V | · · | GND
 *        RESET | · · | PF12(SD_DETECT)             (LCD_D7)  PG7 | · · | PG6  (LCD_D6)
 *   (MOSI)PB15 | · · | PF11(BTN_EN2)               (LCD_D5)  PG3 | · · | PG2  (LCD_D4)
 *  (SD_SS)PB12 | · · | PG10(BTN_EN1)               (LCD_RS) PD10 | · · | PD11 (LCD_EN)
 *    (SCK)PB13 | · · | PB14(MISO)                 (BTN_ENC)  PA8 | · · | PG4  (BEEPER)
 *                    ￣￣
 *               EXP2                                               EXP1
 */
#ifdef LCD_SD
    //#define SDSS                             PB12
    #define SDSS_PIN_Port                      GPIOB
    #define SDSS_Pin                           GPIO_PIN_12
    //#define SCK_PIN                          PB13
    #define SCK_PIN_Port                       GPIOB
    #define SCK_Pin                            GPIO_PIN_13
    //#define MISO_PIN                         PB14
    #define MISO_PIN_Port                      GPIOB
    #define MISO_Pin                           GPIO_PIN_14
    //#define MOSI_PIN                         PB15
    #define MOSI_PIN_Port                      GPIOB
    #define MOSI_Pin                           GPIO_PIN_15
    //#define SD_DETECT_PIN                    PF12
    #define SD_DETECT_PIN_Port                 GPIOF
    #define SD_DETECT_Pin                      GPIO_PIN_12
#endif

/*********************LCD模块按键引脚定义*******************/
//#define BEEPER_PIN                           PG4
#define BEEPE_PIN_Port                         GPIOG
#define BEEPE_Pin                              GPIO_PIN_4
//#define BTN_ENC                              PA8
#define BTN_ENC_PIN_Port                       GPIOA
#define BTN_ENC_Pin                            GPIO_PIN_8
//#define BTN_A                                PG10
#define BTN_A_PIN_Port                       	 GPIOG
#define BTN_A_Pin                              GPIO_PIN_10
//#define BTN_B                                PF11
#define BTN_B_PIN_Port                       	 GPIOF
#define BTN_B_Pin                              GPIO_PIN_11

/*********************LCD模块显示引脚定义*******************/
//#define ST7920_CLK_PIN                       PG2
#define ST7920_CLK_PIN_Port                 	 GPIOG
#define ST7920_CLK_Pin                      	 GPIO_PIN_2
//#define ST7920_DAT_PIN                       PD11
#define ST7920_DAT_PIN_Port                 	 GPIOD
#define ST7920_DAT_Pin                      	 GPIO_PIN_11
//#define ST7920_CS_PIN                        PD10
#define ST7920_CS_PIN_Port                 	   GPIOD
#define ST7920_CS_Pin                      	   GPIO_PIN_10


#endif

