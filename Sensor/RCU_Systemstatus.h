#ifndef RCU_SYSTEMSTATUS_HEADER
#define RCU_SYSTEMSTATUS_HEADER

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

class RCU_Systemstatus :
	public Sensor
{
private:
	FSM_Controller* rcuPtr;
	std::atomic<double> rcu_uplink_obc;
	std::atomic<double> rcu_downlink_obc;
	std::atomic<double> pg_hv;
	std::atomic <double> pg_5v;
	std::atomic<bool> rcu_sim_mode;
public:
	RCU_Systemstatus(float updateFreq, FSM_Controller* rcu);
	void init() override;
	void update() override;
    std::unique_ptr<Data> getData() override;
	int getI2CAddr() override;
    SensorType getSensorType() override;
};

#endif