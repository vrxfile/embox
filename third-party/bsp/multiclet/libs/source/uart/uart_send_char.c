#include "uart.h"
/**
  * @brief  Посылка байта по UART.
  * @param  byte : 8 разрядное значение для вывода по UART.
  * @param  UART : Выбор номера UART для вывода данных.
  * Данный параметр может принимать одно из следующих значений:
  * UART0, UART1, UART2, UART3.
  * @retval
  * Нет
  */
void uart_send_char(unsigned char byte, UART_TypeDef *UART)
{
        while(UART_FIFO_TX_FULL(UART) == 1);
		UART_SEND_BYTE(byte, UART);
}
