#include "LaserDist_VL530LX.h"

LaserDist_VL530LX::LaserDist_VL530LX(float updateFreq, SensorType sensor_type, int i2c_address) : Sensor(updateFreq)
{
	sensor_type = sensor_type;
	I2C_ADDRESS = i2c_address;
}

Data* LaserDist_VL530LX::getData()
{
	return sensorData;
}

int LaserDist_VL530LX::getI2CAddr()
{
	return I2C_ADDRESS;
}

SensorType LaserDist_VL530LX::getSensorType()
{
	return sensor_type;
}
