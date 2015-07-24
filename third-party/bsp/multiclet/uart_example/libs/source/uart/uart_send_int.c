#include "uart.h"
/**
  * @brief  Посылка строки по UART.
  * @param  str : 32 разрядное значение для вывода по UART.
  * @param  UART : Выбор номера UART для вывода данных.
  * Данный параметр может принимать одно из следующих значений:
  * UART0, UART1, UART2, UART3.
  * @retval 
  * Нет
  */
void uart_send_int(int str, UART_TypeDef *UART)
{
	int i;
	for(i=0;i<4;i++)
	{
        while(UART_FIFO_TX_FULL(UART) == 1);
		UART_SEND_BYTE(str, UART);
		str = str >> 8;
	}
}
