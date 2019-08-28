/**
* @file adc.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "rotEnc.h"
#include "Roach2_Sensor_Sensor.h"
// TODO: INIT, UPDATE, GETDATA
rotEnc_LS7366R::rotEnc_LS7366R()
{
	this->deviceHandle = wiringPiSPISetup(LS7366R_CHANNEL, speed); // Get file/I2C handle
	if (this->deviceHandle) == -1)
	{
		stdout << "rotEnc: Connection failed!"
	}
}

void rotEnc_LS7366R::init()
{
	//TODO more init, or merge init and tempConfig
	this->tempConfig();
}

void rotEnc_LS7366R::update()
{
//	int wiringPiSPIDataRW(int channel, unsigned char *data, int length)
}

Data rotEnc_LS7366R::getdata() {
	return measurement;
	/*Effekt der eigenerhitzung wird vernachlässigt*/
}

int rotEnc_LS7366R::getI2CAddr() {
//	return MCP3428_DEVICE_ID;
}

void rotEnc_LS7366R::tempConfig()
{
//	simpleWrite(CONF_ADC);
}