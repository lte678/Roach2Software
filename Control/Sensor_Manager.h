#ifndef HEADER_SENSOR_MANAGER
#define HEADER_SENSOR_MANAGER

#include <thread>
#include <atomic>
#include <cmath>
#include <mutex>

#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <fstream>

#include "../Sensor/Sensor.h"
#include "../Sensor/IMU.h"
#include "../DataStore/data.h"
#include "../Sensor/Temp.h"
#include "../Sensor/ARM_Systeminfo.h"
#include "../Sensor/ADC.h"
#include "../Sensor/RotaryEncoder.h"
#include "../Sensor/OBC_Systemstatus.h"
#include "../Sensor/RCU_Systemstatus.h"
#include "../Sensor/RocketSignals.h"

class Sensor_Manager
{
private:
    std::mutex sensor_lock;
	std::vector<Sensor*> sensors; // List of all sensor objects
	std::vector<std::thread> sensorThreads;
	std::atomic<int> number_sensors; // Total number of sensors used
	std::atomic<bool> stop_bit;
	float logging_rate; // Logging rate in Hz
	const std::string filename_logging = "log"; // Without file extension, will be set to .csv
	int logging_stream;
public:
	Sensor_Manager();
	// Threading related
	void run();
	void stop();
	Sensor* getSensorHandle(SensorType sensor_id);
	void attachSensor(Sensor *sensor);
	// Access to loaded sensor data
	bool getData(std::unique_ptr<Data>& data_ptr, SensorType sensor_id);
};

#endif