/**
* @file adc.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "adc.h"

// TODO: INIT, UPDATE, GETDATA
ADC_MCP3428::ADC_MCP3428()
{
	this->deviceHandle = this->i2cConnect(MCP3428_DEVICE_ID); // Get file/I2C handle
	if (read8(MCP3428_DEVICE_ID) != 0x00) 
	{
		std::cout << "ADC: Connection failed!";
	}
}

ADC_MCP3428::~ADC_MCP3428()
{
}

void ADC_MCP3428::init()
{
	simpleWrite(CONF_ADC);
}

void ADC_MCP3428::update()
{
	measurement = simpleRead();

	measurement = measurement >> 5;
	if ((measurement >> 15) != 0)
	{
		int signchange = 0b1000000000000000;
		measurement = measurement | signchange;
		measurement = binaryToDecimal(measurement);
		measurement = measurement * -1;
	}
	else
	{
		measurement = binaryToDecimal(measurement);
	}
	measurement = measurement * 0.0625;
	/*hier wird measurement in float umgewandelt in mV*/

	this->data_obj = new Data();
	this->data_obj->addValue("ADC", measurement);
}

Data* ADC_MCP3428::getData() {
	return this->data_obj;
			/*Effekt der eigenerhitzung wird vernachlässigt*/
}

int ADC_MCP3428::getI2CAddr() {
	return MCP3428_DEVICE_ID;
}

