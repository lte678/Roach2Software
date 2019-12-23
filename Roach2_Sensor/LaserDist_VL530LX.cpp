#include "LaserDist_VL530LX.h"

LaserDist_VL530LX::LaserDist_VL530LX(int sensor_type, int i2c_address)
{
	this->sensor_type = sensor_type;
	this->I2C_ADDRESS = i2c_address;
}

Data* LaserDist_VL530LX::getData()
{
	return this->sensorData;
}

int LaserDist_VL530LX::getI2CAddr()
{
	return this->I2C_ADDRESS;
}

int LaserDist_VL530LX::getSensorType()
{
	return this->sensor_type;
}
