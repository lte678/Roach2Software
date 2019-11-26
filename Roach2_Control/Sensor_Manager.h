#ifndef HEADER_SENSOR_MANAGER
#define HEADER_SENSOR_MANAGER

#include <thread>
#include <atomic>
#include <math.h>
#include <mutex>

#include "../Roach2_Sensor/Roach2_Sensor_Sensor.h"
#include "../Roach2_Sensor/imu_neu.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_Sensor/Temp.h"
#include "../Roach2_Sensor/ARM_Systeminfo.h"


class Sensor_Manager
{
private:
	std::vector<Sensor*> sensors; // List of all sensor objects
	int number_sensors; // Total number of sensors used
	std::atomic<bool> stop_bit;
	std::vector<Data*> loaded_data;
	int update_rate; // Update rate in Hz
	std::mutex lock_data_access; // Mutex to prohibit problems with access data from multiple threads
	int sensor_values_loaded;
public:
	Sensor_Manager(bool obc, bool rcu);
	// Threading related
	void run(void);
	void stop(void);
	// Access to loaded sensor data
	bool getData(Data** data_ptr, SENSOR_TYPES sensor_id);
	bool getData(Data** data_ptr);
	void setUpdateRate(int update_rate);
	int getStatNumberSensorValuesLoaded(void);
};

#endif