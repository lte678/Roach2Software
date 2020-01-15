#include "Temp.h"

TEMP_LM75B::TEMP_LM75B(float updateFreq) : Sensor(updateFreq)
{
    std::cout << "[Sensor|Temp] Initializing" << std::endl;
	convertedMeasurement = 0.0;
}

void TEMP_LM75B::init() {
    this->deviceHandle = i2cConnect(LM75B_DEVICE_ID); // Get file/I2C handle
    write16(TOS_reg, TOS_TEMP);
    write16(THYST_reg, THYST_TEMP);
    write8(CONF_reg, CONF_TEMP);
}

void TEMP_LM75B::update()
{
    // Retrieve temperature data
	int16_t measurement = read16(TEMP_reg);

	// Switch byte order, so it goes from MSB to LSB
    measurement = ((measurement & 0xFF) << 8) | ((measurement & 0xFF00) >> 8);
    measurement = measurement >> 5;

    // Check if negative number
    if(measurement & 0x0400) {
        // Sign extension with ones
        measurement |= 0xF800;
    }

    // Final temperature value in degrees celsius
	convertedMeasurement = measurement * 0.125;
}

std::unique_ptr<Data> TEMP_LM75B::getData() {
    std::unique_ptr<Data> data_ptr(new Data());
	data_ptr->setId((int)SensorType::TEMP_SENSOR);
	data_ptr->addValue("TEMP", convertedMeasurement);
	return data_ptr;
	/*Effekt der eigenerhitzung wird vernachlaessigt*/
}

int TEMP_LM75B::getI2CAddr() {
	return LM75B_DEVICE_ID;
}

SensorType TEMP_LM75B::getSensorType()
{
	return SensorType::TEMP_SENSOR;
}

