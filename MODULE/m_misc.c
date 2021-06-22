#include "m_misc.h"

void m_serialprintPGM(const char *str)
{
  while (*str)
  {
    uint8_t temp[1]={*str};
    HAL_UART_Transmit(&UartData.handle, temp, 1, 1000);
    str++;
  }
}

void m_printPGM(const char *str)
{
  printf(str);
}

void m_serialprintlong(long c)
{
  printf("%ld\r\n", c);
}

