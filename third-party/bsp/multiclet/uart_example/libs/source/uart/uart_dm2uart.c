#include "uart.h"
/*
Variable for DM2UART function:

DM_START - start block address for send by UART
BLOCK_SIZE - end block address for send by UART  
iterator1 - number one byte for send by UART
iterator2 - counter fifo UART 
byte_DM - contain byte for send by UART 
*/

/**
  * @brief  Чтение очередного байта данных из ПД(памяти данных). 
  * @retval 
  * Возвращаемые значения: 	 
  * - 1 : все байты блока считаны
  * - 0 : считаны не все байты блока
  */
int read_DM(int *iterator1, int *iterator2, int block_size, char *byte_dm, int dm_start)
{
		if(block_size + dm_start - *iterator1 != 0)
		{	
			if(*iterator1 == 0) *iterator1 = *iterator1 + dm_start;
			*byte_dm = *((char*)(*iterator1)); //read 1 byte from DM
			*iterator1 = *iterator1 + 1;
			*iterator2 = *iterator2 - 1;
			return 0;
		}
		else
		{
			return 1;
		}
}
/**
  * @brief  Проверка на заполнение буфера fifo прочитанными данными (проверка прихода 32 элементов). 
  * Возвращаемые значения: 	 
  * - 1 : в буфер fifo передатчика положены 32 байта
  * - 0 : в буфер fifo не отправлены все 32 байта
  */
int full_fifo(int *iterator2)
{
	if(*iterator2 == 0)
	{
		*iterator2 = 32;
		return 1;
	}
	else
	{
		return 0;
	}
}
/**
  * @brief  Функция вывода по UART блока байтов ПД(памяти данных).
  * @param  UART : Выбор необходимого номера UART.
  * Данный параметр может принимать одно из следующих значений:
  * UART0, UART1, UART2, UART3.
  * @param  start_address : Значение для начала блока считывания байтов
  * информации из ПД(памяти данных) и вывода их по UART.
  * @param  size_block : Размер блока данных в байтах для вывода по UART.
  * @retval Нет
  */
void DM2UART(UART_TypeDef *UART, int start_address, int size_block)
{
	int iterator1, iterator2;
	int	dm_start, block_size;
	char byte_dm;
	iterator2 = 32;
	iterator1 = 0;
	dm_start = start_address;
	block_size = size_block;
    while(UART_FIFO_TX_EMPTY(UART) != 1);
 
	while(read_DM(&iterator1, &iterator2, block_size,&byte_dm, dm_start) == 0)
	{
		UART_SEND_BYTE(byte_dm, UART);
		
		if(full_fifo(&iterator2) == 1)
		{
			while(UART_FIFO_TX_EMPTY(UART) != 1);	  
			
		}
	}
}
