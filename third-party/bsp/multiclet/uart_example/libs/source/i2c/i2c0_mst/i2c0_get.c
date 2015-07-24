#include "i2c.h"
/**
  * @brief  Прием байта по i2c0
  * @param  byte : байт на передачу по i2c0. 
  * @param  slv_addr : адрес ведомого устройства i2c0.
  * @param  addr : адрес памяти ведомого устройства i2c0.
  * @retval Нет
  */
char i2c0_get(char slv_addr, int addr)
{
	I2C0_TX_DATA(slv_addr); //set Slave Address for transmit
	I2C0_CMD(I2C0_START_WR); //send START + WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK
	
	I2C0_TX_DATA((addr & 0xFF00)>>8); //set first address byte for transmit
	I2C0_CMD(I2C0_WR); //send WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK
	
	I2C0_TX_DATA(addr & 0xFF); //set second address byte for transmit
	I2C0_CMD(I2C0_WR); //send WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK
	

	I2C0_TX_DATA(slv_addr+1); //set Slave Address for transmit
	I2C0_CMD(I2C0_START_WR); //send START + WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK
	
	I2C0_CMD(I2C0_NACK_RD_STOP); //send NACK + RD + STOP
	while(I2C0_TIP); //wait end of data transmit

	return I2C0_RX_DATA; //read received byte
}
