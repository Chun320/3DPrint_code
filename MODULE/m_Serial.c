#include "m_Serial.h"
#include "string.h"
#include "stdio.h"

UartDataType UartData;
uint8_t UartTxBuffer[2 * UART_TX_BUFFER_SIZE];
uint8_t UartRxBuffer[2 * UART_RX_BUFFER_SIZE];

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}


void m_Serial_Init(uint32_t BaudRate)
{
  UartDataType *Uart = &UartData;

  Uart->handle.Instance = SERIAL1;
  Uart->handle.Init.BaudRate = BaudRate;
  Uart->handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart->handle.Init.StopBits = UART_STOPBITS_1;
  Uart->handle.Init.Parity = UART_PARITY_NONE;
  Uart->handle.Init.Mode = UART_MODE_TX_RX;
  Uart->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  Uart->handle.Init.OverSampling = UART_OVERSAMPLING_16;

  if(HAL_UART_DeInit(&Uart->handle) != HAL_OK)
  {
		printf("����1����λʼ��ʧ�ܣ�\r\n");
  }

  if(HAL_UART_Init(&Uart->handle) != HAL_OK)
  {
		printf("����1��ʼ��ʧ��!\r\n");
  }
	
	//�������ڽ����ж�
	m_Serial_start();
}

void m_Serial_start(void)
{
  UartDataType *Uart = &UartData;
  Uart->pRxBuffer = (uint8_t *)UartRxBuffer;
  Uart->pRxWriteBuffer =  Uart->pRxBuffer;
  Uart->pRxReadBuffer =  Uart->pRxBuffer;
  
  Uart->pTxBuffer = (uint8_t *)UartTxBuffer;
  Uart->pTxWriteBuffer =  Uart->pTxBuffer;
  Uart->pTxReadBuffer =  Uart->pTxBuffer;
  Uart->pTxWrap  =  Uart->pTxBuffer + UART_TX_BUFFER_SIZE;
  
  Uart->rxBusy = RESET;
  Uart->txBusy = RESET;
  Uart->debugNbTxFrames = 0;
  Uart->debugNbRxFrames = 0;
  
  Uart->newTxRequestInThePipe = 0;
  Uart->nbBridgedBytes = 0;
  Uart->gCodeDataMode = 0;
    
  if (HAL_UART_Receive_IT(&Uart->handle, (uint8_t *)(&Uart->rxWriteChar), 1) != HAL_OK)
  {
    printf("����1�״δ򿪽����ж�ʧ��!\r\n");
  }  
}

//ͨ�Ŵ���ͨ�ų�ʼ���ص�����������GPIO�ڳ�ʼ����
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  if(huart->Instance == SERIAL1)
  {
    GPIO_InitTypeDef GPIO_InitStruct;

    SERIAL1_CLK_ENABLE();
    SERIAL1_RX_GPIO_CLK_ENABLE();
    SERIAL1_TX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = SERIAL1_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = SERIAL1_TX_AF;
    HAL_GPIO_Init(SERIAL1_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SERIAL1_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = SERIAL1_RX_AF;
    HAL_GPIO_Init(SERIAL1_RX_PORT, &GPIO_InitStruct);

    //�����ж����ȼ�
    HAL_NVIC_SetPriority(SERIAL1_IRQn, 2, 0);
    //ʹ�ܴ���1�ж�ͨ��
    HAL_NVIC_EnableIRQ(SERIAL1_IRQn); 
  }  
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  UartDataType *Uart = &UartData;
  if(UartHandle == &Uart->handle)
  {
    *Uart->pRxWriteBuffer = Uart->rxWriteChar;

    //�򿪴��ڽ����жϣ�Ϊ��һ�ν�����׼��
    if (HAL_UART_Receive_IT(&Uart->handle, (uint8_t *)(&Uart->rxWriteChar), 1) != HAL_OK)
    {
      printf("����1�򿪽����ж�ʧ��!\r\n");
    }
    Uart->pRxWriteBuffer++;
    //�ж���һ����ַ�Ƿ񳬳��趨���ַ�����ռ䣨UART_RX_BUFFER_SIZE��
    if (Uart->pRxWriteBuffer >= (Uart->pRxBuffer + UART_RX_BUFFER_SIZE))
    {
      //���������ָ��ռ䣬��ָ�븳ֵΪ��ʼֵ
      Uart->pRxWriteBuffer = Uart->pRxBuffer;
    }
    if (Uart->pRxWriteBuffer == Uart->pRxReadBuffer)
    {
      printf("����1��ȡ���������!\r\n");
    }    
    
    if (Uart->uartRxDataCallback != 0)
    {
      Uart->uartRxDataCallback((uint8_t *)Uart->pRxReadBuffer,Uart->pRxWriteBuffer - Uart->pRxReadBuffer);
    }
    Uart->debugNbRxFrames++;
  }
}

uint32_t m_UartGetNbRxAvalaibleBytes(void)
{
  UartDataType *Uart = &UartData;  
  uint8_t *writePtr = (uint8_t *)(Uart->pRxWriteBuffer - 1);
  
  if (writePtr < Uart->pRxBuffer)
  {
    writePtr += UART_RX_BUFFER_SIZE;
  }  
  
  if ((int8_t)(*writePtr) != 0XA)
    return (0);
  
  int32_t nxRxBytes = Uart->pRxWriteBuffer - Uart->pRxReadBuffer;
  if (nxRxBytes < 0)
  {
    nxRxBytes += UART_RX_BUFFER_SIZE;
  }
  
  return ((uint32_t) nxRxBytes );
}

//�Ӵ��ڻ�ȡ�ַ�
int8_t m_UartGetNextRxBytes(void)
{
  UartDataType *Uart = &UartData;  
  int8_t byteValue;

  uint8_t *writePtr = (uint8_t *)(Uart->pRxWriteBuffer);
  
  if (writePtr < Uart->pRxBuffer)
  {
    writePtr += UART_RX_BUFFER_SIZE;
  }  
  
  if (Uart->pRxReadBuffer != writePtr)
  {
    byteValue = (int8_t)(*(Uart->pRxReadBuffer));
    Uart->pRxReadBuffer++;

    if (Uart->pRxReadBuffer >= (Uart->pRxBuffer + UART_RX_BUFFER_SIZE))
    {
      Uart->pRxReadBuffer = Uart->pRxBuffer;
    } 
  }
  else
  {
    byteValue = -1;
  }
  
  return (byteValue);
}

void m_UartIfQueueTxData(uint8_t *pBuf, uint8_t nbData)
{
  if (nbData != 0)
  {
    UartDataType *Uart= &UartData;  
    int32_t nbFreeBytes = Uart->pTxReadBuffer - Uart->pTxWriteBuffer;
       
    if (nbFreeBytes <= 0)
    {
      nbFreeBytes += UART_TX_BUFFER_SIZE;
    }
    if (nbData > nbFreeBytes)
    {
        /*����д���������� */
       printf("����1д���������!\r\n");
    }
    
    //use of memcpy is safe as real buffer size is 2 * UART_TX_BUFFER_SIZE
    memcpy((uint8_t *)Uart->pTxWriteBuffer, pBuf, nbData);
    Uart->pTxWriteBuffer += nbData;
    if (pBuf[nbData-1] == '\n')
    {
      *Uart->pTxWriteBuffer = '\n';
      Uart->pTxWriteBuffer--;
      *Uart->pTxWriteBuffer = '\r';
      Uart->pTxWriteBuffer += 2;
      if (Uart->pTxWriteBuffer >= Uart->pTxBuffer + UART_TX_BUFFER_SIZE)
      {
        Uart->pTxWrap = Uart->pTxWriteBuffer; 
        Uart->pTxWriteBuffer = Uart->pTxBuffer;
      }        
    }
  }
}

void m_UartIfSendQueuedData(void)
{
    UartDataType *Uart = &UartData;  

    if ((Uart->newTxRequestInThePipe == 0)&&
        (Uart->txBusy == RESET)&&
        (Uart->pTxReadBuffer != Uart->pTxWriteBuffer))
    {
      int32_t nbTxBytes = Uart->pTxWriteBuffer - Uart->pTxReadBuffer;
      Uart->newTxRequestInThePipe++;
      if (nbTxBytes < 0)
      {
        nbTxBytes = Uart->pTxWrap - Uart->pTxReadBuffer;
      }
      if (Uart->pTxReadBuffer[nbTxBytes-1]!='\n')
      {
        Uart->newTxRequestInThePipe--;
        return;
      }
      Uart->txBusy = SET;
      Uart->nbTxBytesOnGoing = nbTxBytes;       
      
      //use of HAL_UART_Transmit_IT is safe as real buffer size is 2 * UART_TX_BUFFER_SIZE
      if(HAL_UART_Transmit_IT(&Uart->handle, (uint8_t *) Uart->pTxReadBuffer, nbTxBytes)!= HAL_OK)
      {
				printf("����1�򿪷����ж�ʧ��!\r\n");
      }
      
      Uart->debugNbTxFrames++;
      Uart->newTxRequestInThePipe--;
    }
}

void UartLockingTx(uint8_t *pBuf, uint8_t nbData)
{
  UartDataType *Uart = &UartData;
  HAL_UART_Transmit(&Uart->handle, pBuf, nbData, 1000);
}
