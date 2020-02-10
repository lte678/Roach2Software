#include "LaserDist_VL530LX.h"

LaserDist_VL530LX::LaserDist_VL530LX(float updateFreq, SensorType sensor_type, int i2c_address) : Sensor(updateFreq)
{
	sensor_type = sensor_type;
	I2C_ADDRESS = i2c_address;
	distance = 0.0f;
}

void LaserDist_VL530LX::init() {
    // Dont do anything yet
}

void LaserDist_VL530LX::update() {
    distance = 0.0f;
}

std::unique_ptr<Data> LaserDist_VL530LX::getData()
{
    std::unique_ptr<Data> data_ptr(new Data());
    data_ptr->setId((int)SensorType::ADC);

    data_ptr->addValue("DIST", distance);

    return data_ptr;
}

int LaserDist_VL530LX::getI2CAddr()
{
	return I2C_ADDRESS;
}

SensorType LaserDist_VL530LX::getSensorType()
{
	return sensor_type;
}
