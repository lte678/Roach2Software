#ifndef OBC_SYSTEMSTATUS_HEADER
#define OBC_SYSTEMSTATUS_HEADER

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../DataStore/data.h"
#include "../Comm/EthernetClient.h"
#include "../Comm/EthernetServer.h"
#include "Sensor.h"

class FSM_Controller;

class OBC_Systemstatus :
	public Sensor
{
private:
	FSM_Controller* obcPtr;
    std::atomic<int> obc_uplink_rcu;
    std::atomic<int> obc_downlink_rcu;
    std::atomic<float> obc_sim_mode;
    std::atomic<float> pg_rover;
    std::atomic<float> pg_cam;
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