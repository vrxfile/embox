//SPIx constant file
#ifndef SPI_H
#define SPI_H


#include "R1_ccf.h"
#include "gpio.h"
typedef struct
{
  __IO uint32_t CFG;
  __IO uint32_t RESERVED0;
  __IO uint32_t RESERVED1;
  __IO uint32_t RESERVED2;
  __IO uint32_t RESERVED3;
  __IO uint32_t RESERVED4;
  __IO uint32_t RESERVED5;
  __IO uint32_t RESERVED6;
  __IO uint32_t CR;
  __IO uint32_t ST;
  __IO uint32_t MSK;
  __IO uint32_t CMD;
  __IO uint32_t TX;
  __IO uint32_t RX;
  __IO uint32_t SS;

} SPI_TypeDef;

#define APB1PERIPH_BASE       (0xC0100000)
//SPI0
#define SPI0_BASE            (APB1PERIPH_BASE + 0x00004000)
#define SPI0                 ((SPI_TypeDef *) SPI0_BASE)
//SPI1
#define SPI1_BASE            (APB1PERIPH_BASE + 0x00008000)
#define SPI1                 ((SPI_TypeDef *) SPI1_BASE)

typedef struct
{
 __IO uint32_t Three_order_tx;
 __IO uint32_t SCK_gap;
 __IO uint32_t FACT;
 __IO uint32_t Three_wire_en;
 __IO uint32_t PM;
 __IO uint32_t Word_len;
 __IO uint32_t SPI_en;
 __IO uint32_t Mode_select;
 __IO uint32_t Reverse_data;
 __IO uint32_t Divide16;
 __IO uint32_t Clock_phase;
 __IO uint32_t Clock_polarity;
 __IO uint32_t Loop_mode;

} SPI_InitTypeDef;

//spi constant and macro
//Разрешение сигналов выбора ведомого устройства
#define SPI_SSEN(SPI, EN) SPI->CFG |= ((EN & 0x01) << 16)
//Разрешение трехпроводного режима
#define SPI_TWEN(SPI, EN) SPI->CFG |= ((EN & 0x01) << 19)
//Получение глубины буфера fifo на прием и передачу
#define SPI_READ_FDEPTH(SPI) ((SPI->CFG >> 8) & 0xFF)
//Получение максимального числа линий выборок ведомого устройства
#define SPI_READ_SSSZ(SPI) ((SPI->CFG >> 24) & 0xFF)
//Получение максимальной поддерживаемой длины слова данных
#define SPI_READ_MAXWLEN(SPI) ((SPI->CFG >> 20) & 0x0F)
//Отправка команды на передачу последнего слова данных
#define SPI_CMD_LST(SPI) SPI->CMD = 0x00400000
//Принятые данные
#define SPI_RX_DATA(SPI) SPI->RX
//Данные на передачу
#define SPI_TX_DATA(SPI, TXDATA) SPI->TX = TXDATA
//Очистить бит заполнения буфера приемника и игнорирования новых данных
#define SPI_CLEAR_OV(SPI) (SPI->ST |= (1 << 12));
//Очистить бит передачи последнего слова данных
#define SPI_CLEAR_LT(SPI) (SPI->ST |= (1 << 14));
//Проверка буфера передатчика на наличие свободного места(0 - буфер передатчика заполнен;1 - буфер передатчика не заполнен)
#define SPI_ST_NF(SPI) ((SPI->ST & 0x0100)? 1 : 0)
//Проверка буфера приемника на наличие данных (0 - буфер приемника пуст;1 - буфер приемника содержит данные)
#define SPI_ST_NE(SPI) ((SPI->ST & 0x0200)? 1 : 0)
//Проверка статусного регистра на наличие бита ошибки при работе системы с несколькими ведущими (0 - ошибки нет;1 - ошибка есть)
#define SPI_ST_MME(SPI) ((SPI->ST & 0x0400)? 1 : 0)
//Проверка статусного регистра на наличие бита отсутствия данных для предачи в буфере при запросе ведущего(0 - данные присутствуют;1 - данные отсутствуют)
#define SPI_ST_UN(SPI) ((SPI->ST & 0x0800)? 1 : 0)
//Проверка на заполнение буфера приемника(0 - буфер приемника не заполнен;1 - буфер приемника заполнен)
#define SPI_ST_OV(SPI) ((SPI->ST & 0x1000)? 1 : 0)
//Проверка на передачу последнего слова данных (0 - не передано; 1 - передано)
#define SPI_ST_LT(SPI) ((SPI->ST & 0x4000)? 1 : 0)
//Проверка передаются ли сейчас данные (0 - не передаются; 1 - передаются)
#define SPI_ST_TIP(SPI) ((SPI->ST & 0x80000000)? 1 : 0)


//prototypes
//Инициализация выбранного номера SPI(SPI0...SPI3), сконфигурированного структурой SPI_InitStruct
void spi_init(SPI_TypeDef *SPI, SPI_InitTypeDef* SPI_InitStruct);


#endif
