#include "Temp.h"

TEMP_LM75B::TEMP_LM75B()
{
	this->deviceHandle=i2cConnect(LM75B_DEVICE_ID); // Get file/I2C handle
}

void TEMP_LM75B::init()
{
	//TODO more init, or merge init and tempConfig
	this->tempConfig();
}

void TEMP_LM75B::update()
	{
	measurement = read16(CONF_TEMP);
	
	measurement = measurement >> 5;
	if ((measurement >> 10) != 0)
	{
		int signchange = 0b10000000000;
		measurement = measurement | signchange;
		measurement = binaryToDecimal(measurement);
		measurement = measurement * -1;
	}
	else
	{
		measurement = binaryToDecimal(measurement);
	}
	measurement = measurement * 0.125;
	/*hier wird measurement in float umgewandelt in °C*/
}

Data* TEMP_LM75B::getData() {
	Data* data_ptr = new Data();
	data_ptr->addValue("TEMP", measurement);
	return data_ptr;
		/*Effekt der eigenerhitzung wird vernachlässigt*/
}

int TEMP_LM75B::getI2CAddr() {
	return LM75B_DEVICE_ID;
}

void TEMP_LM75B::tempConfig()
{
	write16(TOS_reg, TOS_TEMP);
	write16(THYST_reg, THYST_TEMP);
	write8(CONF_reg, CONF_TEMP);
}

