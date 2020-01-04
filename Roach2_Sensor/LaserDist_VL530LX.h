#ifndef LASER_DIST_VL53L0X_HEADER_FILE
#define LASER_DIST_VL53L0X_HEADER_FILE

#include "Roach2_Sensor_Sensor.h"
#include "../Roach2_DataStore/data.h"

#include "STVL53L0X/core/inc/vl53l0x_api.h"
#include "STVL53L0X/platform/inc/vl53l0x_platform.h"

class LaserDist_VL530LX :
	public Sensor
{
private:
	Data* sensorData;
	int I2C_ADDRESS = 0x52;
    SensorType sensor_type;
public:
	LaserDist_VL530LX(float updateFreq, SensorType sensor_type, int i2c_address);
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
	/**
	* @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
	* @return int sensor identifier
	*/
    SensorType getSensorType();
};

#endif