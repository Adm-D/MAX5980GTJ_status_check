#include "uart.h"

void SendBuf_UART2(char *buf)
{
    for (uint8_t i = 0; i < UART_BUF_LEN; i++)
    {
      if(buf[i] == 0) return;
      while (!LL_USART_IsActiveFlag_TXE(USART2));
      LL_USART_TransmitData8(USART2, buf[i]);
    }
}
