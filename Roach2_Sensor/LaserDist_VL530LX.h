#ifndef LASER_DIST_VL53L0X_HEADER_FILE
#define LASER_DIST_VL53L0X_HEADER_FILE

#include "Roach2_Sensor_Sensor.h"
#include "../Roach2_DataStore/data.h"

class LaserDist_VL530LX :
	public Sensor
{
private:
	int i2cAddress = 0x52;
    SensorType sensorType;
    float distance;
public:
	LaserDist_VL530LX(float updateFreq, SensorType sensor_type, int i2c_address);
	void init() override;
	void update() override;
    std::unique_ptr<Data> getData() override;
	int getI2CAddr() override;
	/**
	* @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
	* @return int sensor identifier
	*/
    SensorType getSensorType() override;
};

#endif