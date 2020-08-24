#include "LaserDist_VL530LX.h"

LaserDist_VL530LX::LaserDist_VL530LX(float updateFreq, SensorType _sensor_type, int i2c_address) : Sensor(updateFreq), sensorType(_sensor_type)
{
    std::cout << "[Sensor|Laser Distance] Initializing" << std::endl;
    std::cout << "[Sensor|Laser Distance] id: " << (int)sensorType << ", address: " << i2c_address << std::endl;
    i2cAddress = i2c_address;
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
    data_ptr->setId((int)sensorType);

    data_ptr->addValue("DIST", distance);

    return data_ptr;
}

int LaserDist_VL530LX::getI2CAddr()
{
	return i2cAddress;
}

SensorType LaserDist_VL530LX::getSensorType()
{
	return sensorType;
}
