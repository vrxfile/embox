//UARTx constant file

#ifndef UART_H
#define UART_H


#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t DATA;
  __IO uint32_t ST;
  __IO uint32_t CR;
  __IO uint32_t BDR;
  
} UART_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)
//UART0
#define UART0_BASE            (APB0PERIPH_BASE + 0x00000100)
#define UART0                 ((UART_TypeDef *) UART0_BASE)
//UART1
#define UART1_BASE            (APB0PERIPH_BASE + 0x00000200)
#define UART1                 ((UART_TypeDef *) UART1_BASE)
//UART2
#define UART2_BASE            (APB1PERIPH_BASE + 0x00000100)
#define UART2                 ((UART_TypeDef *) UART2_BASE)
//UART3
#define UART3_BASE            (APB1PERIPH_BASE + 0x00000200)
#define UART3                 ((UART_TypeDef *) UART3_BASE)

typedef struct
{
 __IO uint32_t System_freq;
 __IO uint32_t BaudRate;     
 __IO uint32_t TypeParity;
 __IO uint32_t Parity;
 __IO uint32_t Mode;
 __IO uint32_t FlowControl;
 __IO uint32_t Loop;
 __IO uint32_t Irq_get_byte;
 __IO uint32_t Irq_send_byte;
 __IO uint32_t Irq_fifo_t;
 __IO uint32_t Irq_fifo_r;
 __IO uint32_t Irq_get_break;
 __IO uint32_t Irq_wait_r;
 __IO uint32_t Irq_shift_empty;
 
} UART_InitTypeDef;


//uart constant and macro
//Посылка байта данных в переменной byte по выбранному номеру UART
#define UART_SEND_BYTE(BYTE, UART) UART->DATA = (BYTE) //send byte and select UART number for it
//Проверка буфера fifo передатчика на полноту (0 - не заполнен;1 - заполнен)
#define UART_FIFO_TX_FULL(UART) ((UART->ST & 0x0200)? 1 : 0) //value fifo_tx_full bit
//Проверка буфера fifo передатчика на отсутствие данных на передачу (0 - не пуст;1 - пуст)
#define UART_FIFO_TX_EMPTY(UART) ((UART->ST & 0x04)? 1 : 0) //value fifo_tx_empty bit
//Получение байта из буфера fifo приемника
#define UART_GET_BYTE(UART) UART->DATA //send byte and select UART number for it
//Проверка UART на получение новых байтов информации (0 - новых данных нет;1 - доступны новые данные)
#define UART_NEW_DATA(UART) ((UART->ST & 0x01)? 1 : 0) //value uart_new_data bit
//Проверка регистра сдвига передатчика UART на пустоту (0 - не пуст;1 - пуст)
#define UART_SHIFT_REG_EMPTY(UART) ((UART->ST & 0x02)? 1 : 0) //value uart_shift_reg_empty bit
//Проверка на получение специального символа завершения обмена BREAK (0 - не получен ;1 - получен)
#define UART_BREAK_DETECT(UART) ((UART->ST & 0x08)? 1 : 0) //value uart_break_detect bit
//Проверка на потерю одного или нескольких символов принятых данных из-за переполнения(0 - нет потерь;1 - есть потери)
#define UART_LOSE_DATA(UART) ((UART->ST & 0x10)? 1 : 0) //value uart_lose_data bit
//Ошибка контроля четности принятых данных (0 - нет ошибки;1 - есть ошибка) 
#define UART_PARITY_ERROR(UART) ((UART->ST & 0x20)? 1 : 0) //value uart_parity_error bit
//Ошибка формата принятых данных (0 - нет ошибки;1 - есть ошибка) 
#define UART_DATAFORM_ERROR(UART) ((UART->ST & 0x40)? 1 : 0) //value uart_dataform_error bit
//Буфер fifo передатчика заполнен на половину или более половины (0 - не заполнен на половину; 1 - заполнен на половину или более)
#define UART_FIFO_TX_FULL_MOREHALF(UART) ((UART->ST & 0x80)? 1 : 0) //value uart_tx_full_morehalf bit
//Буфер fifo приемника заполнен на половину или более половины (0 - не заполнен на половину; 1 - заполнен на половину или более)
#define UART_FIFO_RX_FULL_MOREHALF(UART) ((UART->ST & 0x100)? 1 : 0) //value uart_rx_full_morehalf bit
//Буфер fifo приемника заполнен (0 - не заполнен;1 - заполнен)
#define UART_FIFO_RX_FULL(UART) ((UART->ST & 0x0400)? 1 : 0) //value fifo_rx_full bit
//Получение значения регистра управления
#define UART_READ_CONFIG(UART) UART->CR //read CR register
//Получение значения делителя тактовой частоты
#define UART_READ_BDR(UART) UART->BDR //read BDR register

//prototypes
//Инициализация выбранного номера UART(UART0...UART3), сконфигурированного структурой UART_InitStruct
void uart_init(UART_TypeDef *UART, UART_InitTypeDef* UART_InitStruct);
//Вывод ПД через выбранный UART начиная с start_address размером size_block
void DM2UART(UART_TypeDef *UART, int start_address, int size_block);
//Посылка строки по UART
void uart_send_str(char *str,UART_TypeDef *UART);
//Посылка int по UART
void uart_send_int(int str,UART_TypeDef *UART);
//Посылка char по UART
void uart_send_char(unsigned char byte,UART_TypeDef *UART);


#endif

















