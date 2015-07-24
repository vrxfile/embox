#include "wdt.h"
/**
  * @brief  : Установка циклического режима работы wdt.
  * @param  period: значение периода работы wdt.
  * @retval : нет.
  */
void wdt_set_cycle(int period)
{
	WDT_OFF;
	WDT_SET_CNT_EN;
	WDT_SET_CNT(0x000FFFFF);
	WDT_ON;
}
