#ifndef HEADER_SENSOR_MANAGER
#define HEADER_SENSOR_MANAGER

#include <thread>
#include <atomic>
#include <math.h>
#include <mutex>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <fstream>

#include "../Roach2_Sensor/Roach2_Sensor_Sensor.h"
#include "../Roach2_Sensor/IMU.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_Sensor/Temp.h"
#include "../Roach2_Sensor/ARM_Systeminfo.h"
#include "../Roach2_Sensor/Roach2_Sensor_adc.h"
#include "../Roach2_Sensor/Roach2_Sensor_rotEnc.h"
#include "../Roach2_Sensor/OBC_Systemstatus.h"
#include "../Roach2_Sensor/RCU_Systemstatus.h"
#include "../Roach2_Sensor/RocketSignals.h"

class Sensor_Manager
{
private:
    std::mutex sensor_lock;
	std::vector<Sensor*> sensors; // List of all sensor objects
	std::vector<std::thread> sensorThreads;
	std::atomic<int> number_sensors; // Total number of sensors used
	std::atomic<bool> stop_bit;
	float update_rate; // Update rate in Hz
	float fast_update_rate; // Update rate in Hz for high rate measurements
	float logging_rate; // Logging rate in Hz
	const std::string filename_logging = "log"; // Without file extension, will be set to .csv
	std::ofstream* logging_stream;
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