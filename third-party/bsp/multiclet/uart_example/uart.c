 /**
   ******************************************************************************
   *  _ _ _    _ _ _   _ _
   * |     \  /     |/     \
   * |      \/           _ _\
   * |                  /
   * |         MultiClet
   * |                  \ _ _
   * |                      /
   * |_ _ _|\/|_ _ _|\ _ _ /
   *
   * @file    uart.c 
   * @author  Команда разработчиков Мультиклет
   * @date    13-07-2015
   * @brief   Пример работы c UART. 
   *          
   ******************************************************************************
   * @attention
   *
   * <h2><center>&copy; COPYRIGHT(c) 2015 Multiclet corp.</center></h2>
   *
   *  ДАННЫЙ ПРИМЕР НАПИСАН ДЛЯ БЫСТРОГО ОЗНАКОМЛЕНИЯ ПОЛЬЗОВАТЕЛЯ С
   *  ВОЗМОЖНОСТЯМИ МУЛЬТИКЛЕТОЧНОГО ПРОЦЕССОРА. КОМПАНИЯ МУЛЬТИКЛЕТ НЕ НЕСЁТ
   *  ОТВЕТСТВЕННОСТИ ЗА ВОЗМОЖНЫЙ ПРЯМОЙ ИЛИ КОСВЕННЫЙ УЩЕРБ, ПРИЧИНЁННЫЙ 
   *  ИСПОЛЬЗОВАНИЕМ В ПОЛЬЗОВАТЕЛЬСКИХ ПРОДУКТАХ ИНФОРМАЦИИ, СОДЕРЖАЩЕЙСЯ
   *  В ДАННОМ ПРИМЕРЕ.
   * 
   ******************************************************************************
   */

//Подключаем необходимые библиотеки
#include <R1_ccf.h>
#include <uart.h>

/**
расположение светодиодов: 
LED0 - GPIOB(1<<25), LED1 - GPIOB(1<<24), LED2 - GPIOB(1<<23)
LED3 - GPIOB(1<<27), LED4 - GPIOC(1<<26), LED5 - GPIOB(1<<29)
LED6 - GPIOB(1<<28), LED7 - GPIOA(1<<25)

расположение UART: 
UART3[TXD] - GPIOF(1<<14), UART3[RXD] - GPIOF(1<<13)
UART1[TXD] - GPIOA(1<<23), UART1[RXD] - GPIOA(1<<22), UART1[CTS] - GPIOA(1<<24), UART1[RTS] - GPIOA(1<<25)
UART2[TXD] - GPIOC(1<<11), UART2[RXD] - GPIOC(1<<10)
UART0[TXD] - GPIOC(1<<22), UART0[RXD] - GPIOC(1<<21)
*/

char test[5]={'t', 'e', 's', 't', '\n'};

void main()
{
	
	UART_InitTypeDef UART_InitStructure;
    
	UART_InitStructure.System_freq = 8000000; //указываем системную частоту (8 МГц)
	UART_InitStructure.BaudRate = 115200; //устанавливаем частоту работы UART
	UART_InitStructure.TypeParity = 0x00000000; //указываем тип контроля четности
	UART_InitStructure.Parity = 0x00000000; //запрещаем контроль четности
	UART_InitStructure.FlowControl = 0x00000000; //запрещаем cts/rts
	UART_InitStructure.Mode = 0x00000003; //разрешаем прием и передачу

	GPIOC->BPS = (1<<10)|(1<<11);		 //настраиваем порт F на альтернативную функцию для UART3
	uart_init(UART0, &UART_InitStructure); //инициализируем UART3 с помощью структуры

	DM2UART(UART0, (int)test, 5); //отправляем по UART3 массив из 5-ти элементов
#if 0
	uart_send_int(0xABCDEF12, UART3); //отправляем 32-х битное число по UART3
	*(int*)0x40000 = 0x12345678; //записываем значение в память по адресу 0x40000
	while(UART_FIFO_TX_FULL(UART3) == 1); //проверяем, что в буфере FIFO на передачу есть место
	UART_SEND_BYTE(*(char*)0x40001, UART3); //отправляем байт в буфер FIFO на передачу UART3
	while(UART_FIFO_TX_FULL(UART3) == 1); 
	UART_SEND_BYTE(0xCD, UART3);
	while(UART_FIFO_TX_FULL(UART3) == 1); 
	UART_SEND_BYTE(0xEF, UART3);
	while(UART_FIFO_TX_FULL(UART3) == 1); 
	UART_SEND_BYTE(0x12, UART3);
#endif
	//led    
	GPIOB->DIR |= 1<<25; //настраиваем порт B для светодиода LED0 на выход
	GPIOB->OUT |= 1<<25; //устанавливаем "1" в регистре передаваемых данных порта для LED0
}
