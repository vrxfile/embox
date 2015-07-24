#include "i2c.h"
/**
  * @brief  Настройка I2C1(slave) в соответствии с параметрами
  (Пользователь самостоятельно конфигурирует порты ввода-вывода).
  * @param  enable : Разрешение работы I2C1(slave).
  *   Данный параметр может принимать одно из следующих значений:
  * - 1 : работа I2C1(slave) разрешена
  * - 0 : работа I2C1(slave) запрещена
  * @param  transmit_always_valid : Передача данных всегда разрешена.
  * I2C1 не очищает бит TV, если "1".
  * @param  transmit_valid : подтверждение готовности данных для передачи.
  * I2C1 автоматически очищает бит после передачи байта.
  * @param  tmod : Режим передачи I2C1(slave).
  * @param  rmod : Режим приема I2C1(slave).
  * @retval Нет
  */
void i2c1_config(char enable, char transmit_always_valid, char transmit_valid, char tmod, char rmod)
{
	I2C1->CR = (rmod<<4)|(tmod<<3)|(transmit_valid<<2)|(transmit_always_valid<<1)|(enable); //config control register
}
