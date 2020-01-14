#ifndef RCU_SYSTEMSTATUS_HEADER
#define RCU_SYSTEMSTATUS_HEADER

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

class RCU_Systemstatus :
	public Sensor
{
private:
	EthernetClient* eth_client;
	EthernetServer* eth_server;
	std::atomic<double> rcu_uplink_obc;
	std::atomic<double> rcu_downlink_obc;
	std::atomic<double> pg_hv;
	std::atomic <double> pg_5v;
public:
	RCU_Systemstatus(float updateFreq, EthernetClient* client, EthernetServer* server);
	void init() override;
	void update() override;
    std::unique_ptr<Data> getData() override;
	int getI2CAddr() override;
    SensorType getSensorType() override;
};

#endif