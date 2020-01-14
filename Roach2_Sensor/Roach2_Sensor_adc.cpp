/**
* @file adc.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "Roach2_Sensor_adc.h"

// TODO: INIT, UPDATE, GETDATA
ADC_MCP3428::ADC_MCP3428(float updateFreq) : Sensor(updateFreq)
{
    std::cout << "[Sensor|ADC] Initializing" << std::endl;
	deviceHandle = i2cConnect(MCP3428_DEVICE_ID); // Get file/I2C handle
	if (read8(MCP3428_DEVICE_ID) != 0x00) 
	{
		std::cout << "[Sensor|ADC] Connection failed!" << std::endl;
	}
}

ADC_MCP3428::~ADC_MCP3428()
{
}

void ADC_MCP3428::init()
{
	simpleWrite(CONF_ADC1);
    address[0] = CONF_ADC1;
    address[1] = CONF_ADC2;
    address[2] = CONF_ADC3;
    address[3] = CONF_ADC4;
}

void ADC_MCP3428::update()
{
	for (int ch = 0; ch<=3; ch++) {
		measurement[ch] = simpleRead();

//TODO: Stimmt möglicherweise nicht
		measurement[ch] = measurement[ch] >> 5;
		if ((measurement[ch] >> 15) != 0)
		{
			int signchange = 0b1000000000000000;
			measurement[ch] = measurement[ch] | signchange;
			measurement[ch] = binaryToDecimal(measurement[ch]);
			measurement[ch] = measurement[ch] * -1;
		}
		else
		{
			measurement[ch] = binaryToDecimal(measurement[ch]);
		}
		dataLock.lock();
		convertedMeasurement[ch] = measurement[ch] * 0.0625;
		dataLock.unlock();
		/*hier wird measurement in float umgewandelt in mV*/
		simpleWrite(address[ch]);
	}

	simpleWrite(CONF_ADC1);
}

std::unique_ptr<Data> ADC_MCP3428::getData() {
    std::unique_ptr<Data> data_obj(new Data());
    data_obj->setId((int)SensorType::ADC);
    dataLock.lock();
    data_obj->addValue("CH1", convertedMeasurement[0]);
    data_obj->addValue("CH2", convertedMeasurement[1]);
    data_obj->addValue("CH3", convertedMeasurement[2]);
    data_obj->addValue("CH4", convertedMeasurement[3]);
    dataLock.unlock();
	return data_obj;
}

int ADC_MCP3428::getI2CAddr() {
	return MCP3428_DEVICE_ID;
}

SensorType ADC_MCP3428::getSensorType()
{
	return SensorType::ADC;
}

