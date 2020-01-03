#include "Temp.h"

TEMP_LM75B::TEMP_LM75B()
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
    measurement = measurement >> 7;

    // Check if negative number
    if(measurement & 0x0100) {
        // Sign extension with ones
        measurement |= 0xFE00;
    }

    // Final temperature value in degrees celsius
	convertedMeasurement = measurement * 0.5;
}

Data* TEMP_LM75B::getData() {
	Data* data_ptr = new Data();
	data_ptr->setId((int)SENSOR_TYPES::TEMP_SENSOR);
	data_ptr->addValue("TEMP", convertedMeasurement);
	return data_ptr;
	/*Effekt der eigenerhitzung wird vernachlaessigt*/
}

int TEMP_LM75B::getI2CAddr() {
	return LM75B_DEVICE_ID;
}

int TEMP_LM75B::getSensorType()
{
	return SENSOR_TYPES::TEMP_SENSOR;
}

