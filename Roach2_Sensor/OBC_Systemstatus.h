#ifndef OBC_SYSTEMSTATUS_HEADER
#define OBC_SYSTEMSTATUS_HEADER

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../Roach2_DataStore/data.h"
#include "../Roach2_Comm/EthernetClient.h"
#include "../Roach2_Comm/EthernetServer.h"
#include "Roach2_Sensor_Sensor.h"

class FSM_Controller;

class OBC_Systemstatus :
	public Sensor
{
private:
	FSM_Controller* obcPtr;
    std::atomic<int> obc_uplink_rcu;
    std::atomic<int> obc_downlink_rcu;
    std::atomic<bool> obc_sim_mode;
    std::atomic<double> pg_rover;
    std::atomic<double> pg_cam;
public:
	OBC_Systemstatus(float updateFreq, FSM_Controller* obc);
	void init();
	void update();
    std::unique_ptr<Data> getData();
	int getI2CAddr();
	/**
	* @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
	* @return int sensor identifier
	*/
    SensorType getSensorType();
};

#endif