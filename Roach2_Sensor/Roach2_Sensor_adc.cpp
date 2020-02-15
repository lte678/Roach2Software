/**
* @file adc.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "Roach2_Sensor_adc.h"

const int MCP3428_DEVICE_ID = 0x68;
const float LSB_SCALING = 0.0625f;
const int MAX_READ_ATTEMPTS = 10;

// TODO: INIT, UPDATE, GETDATA
ADC_MCP3428::ADC_MCP3428(float updateFreq) : Sensor(updateFreq)
{
    std::cout << "[Sensor|ADC] Initializing" << std::endl;
	deviceHandle = i2cConnect(MCP3428_DEVICE_ID); // Get file/I2C handle

    chSetting[0] = CONF_ADC1;
    chSetting[1] = CONF_ADC2;
    chSetting[2] = CONF_ADC3;
    chSetting[3] = CONF_ADC4;

	if (deviceHandle == 0 || simpleRead() != 0x00)
	{
		std::cout << "[Sensor|ADC] Connection failed!" << std::endl;
	}
}

ADC_MCP3428::~ADC_MCP3428()
{
}

void ADC_MCP3428::init()
{
	//simpleWrite(CONF_ADC1);

}

void ADC_MCP3428::update()
{
    for (int ch = 0; ch<=3; ch++) {
        bool measurementFailed = false;

        // Tell ADC to take measurement
        simpleWrite(chSetting[ch]);

        usleep(5000);

        // Take measurement
        unsigned char data[4];
        int readAttempts = 0;
        do {
            if (readAttempts < MAX_READ_ATTEMPTS) {
                readBlock((char*)data, 4);
                usleep(2000);
                readAttempts++;
            } else {
                measurementFailed = true;
            }
        } while(data[2] & 0b10000000u && !measurementFailed);

        int16_t measurement = data[1];
        measurement |= data[0] << 8;

        measurement = measurement << 4;

        dataLock.lock();
        if(!measurementFailed) {
            convertedMeasurement[ch] = (float)measurement * LSB_SCALING;
        } else {
            convertedMeasurement[ch] = -1.0f;
        }
        dataLock.unlock();
    }
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

