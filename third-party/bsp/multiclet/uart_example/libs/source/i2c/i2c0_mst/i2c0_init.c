#include "i2c.h"
/**
  * @brief  Инициализация I2C0(master) в соответствии с параметрами
  (Пользователь самостоятельно конфигурирует порты ввода-вывода).
  * @param  enable : Разрешение работы I2C0(master). 
  *   Данный параметр может принимать одно из следующих значений:
  * - 1 : работа I2C0 разрешена
  * - 0 : работа I2C0 запрещена
  * @param  clock_speed : Частота работы I2C0(master).
  * @param  system_freq : Частота работы процессора.
  * @retval Нет
  */
void i2c0_init(char enable, int clock_speed, int system_freq)
{
	I2C0->CR = (enable << 7); //enable or disable
	I2C0->PSC = system_freq/(5*clock_speed)-1; //set prescaler for I2C
}
