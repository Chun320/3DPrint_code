#ifndef __M_SERIAL_H
#define __M_SERIAL_H

#include "stdio.h"
#include "sys.h"

#define UART_TX_BUFFER_SIZE (1024)
#define UART_RX_BUFFER_SIZE (1024)

#define SERIAL1                          (USART1)
#define SERIAL1_CLK_ENABLE()              __USART1_CLK_ENABLE()
#define SERIAL1_CLK_DISABLE()             __USART1_CLK_DISABLE()
#define SERIAL1_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define SERIAL1_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define SERIAL1_FORCE_RESET()             __USART1_FORCE_RESET()
#define SERIAL1_RELEASE_RESET()           __USART1_RELEASE_RESET()

/* UART1调试引脚的定义 */
#define SERIAL1_TX_PIN                    (GPIO_PIN_9)
#define SERIAL1_TX_PORT                   (GPIOA)
#define SERIAL1_RX_PIN                    (GPIO_PIN_10)
#define SERIAL1_RX_PORT                   (GPIOA)

#define SERIAL1_TX_AF                     (GPIO_AF7_USART1)
#define SERIAL1_RX_AF                     (GPIO_AF7_USART1)


/* UART1调试中断的定义 */
#define SERIAL1_IRQn                      (USART1_IRQn)
#define SERIAL1_IRQHandler                USART1_IRQHandler  

typedef struct UartDataTag
{
  volatile uint8_t rxWriteChar;
  uint8_t *pRxBuffer;
  volatile uint8_t *pRxWriteBuffer;
  volatile uint8_t *pRxReadBuffer;
  uint8_t *pTxBuffer;
  volatile uint8_t *pTxWriteBuffer;
  volatile uint8_t *pTxReadBuffer;
  volatile uint8_t *pTxWrap;
  volatile uint8_t newTxRequestInThePipe;
  volatile uint8_t gCodeDataMode;
  volatile uint16_t nbTxBytesOnGoing;
  volatile ITStatus rxBusy;
  volatile ITStatus txBusy;
  void (*uartRxDataCallback)(uint8_t *,uint8_t);  
  void (*uartTxDoneCallback)(void);  
  UART_HandleTypeDef handle;
  uint32_t debugNbRxFrames; 
  uint32_t debugNbTxFrames;
  volatile uint32_t nbBridgedBytes;
}UartDataType;

extern UartDataType  UartData;

void m_Serial_Init(uint32_t BaudRate);
void m_Serial_start(void); 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
uint32_t m_UartGetNbRxAvalaibleBytes(void);
int8_t m_UartGetNextRxBytes(void);
void m_UartIfQueueTxData(uint8_t *pBuf, uint8_t nbData);
void m_UartIfSendQueuedData(void);
void UartLockingTx(uint8_t *pBuf, uint8_t nbData);

#endif

