#include "RCU_Systemstatus.h"

RCU_Systemstatus::RCU_Systemstatus(float updateFreq, EthernetClient* client, EthernetServer* server) : Sensor(updateFreq)
{
	this->eth_client = client;
	this->eth_server = server;
}

void RCU_Systemstatus::init()
{
	// Unexport pins (if they were already exported)
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "2", 3); // Power good 5V
		int fd_2 = write(fd_0, "3", 3); // Power good HV
		close(fd_0);
	}

	// Export pins
	fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "2", 3); // Power good 5V
		int fd_2 = write(fd_0, "3", 3); // Power good GoPros/camera
		close(fd_0);
	}

	// Set pins to input
	fd_0 = open("sys/class/gpio/gpio2/direction", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "in", 5);
		close(fd_0);
	}
	fd_0 = open("sys/class/gpio/gpio3/direction", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "in", 5);
		close(fd_0);
	}
}

void RCU_Systemstatus::update()
{
	int buffer = 0;

    pg_5v = 0;
    pg_hv = 0;

	// Load sensor values:
	// Power Good 5V
	int fd_0 = open("sys/class/gpio/gpio2/value", O_RDONLY);
	if (fd_0 != -1) {
		read(fd_0, &buffer, 1); // ASCII 48=0, 49=1
		if (buffer == 49) {
			pg_5v = 1;
		}
		close(fd_0);
	}

	buffer = 0; // Reset buffer

	// Power Good HV
	fd_0 = open("sys/class/gpio/gpio3/value", O_RDONLY);
	if (fd_0 != -1) {
		read(fd_0, &buffer, 1); // ASCII 48=0, 49=1
		if (buffer == 49) {
			pg_hv = 1;
		}
		close(fd_0);
	}

	// Load connection status to OBC
	rcu_uplink_obc = this->eth_client->isConnected() ? 1 : 0;
	rcu_downlink_obc = this->eth_server->isConnected() ? 1 : 0;
}

std::unique_ptr<Data> RCU_Systemstatus::getData()
{
    std::unique_ptr<Data> currentData(new Data());
    currentData->addValue("RCU_UP_OBC", rcu_uplink_obc);
    currentData->addValue("RCU_DOWN_OBC", rcu_downlink_obc);
    currentData->addValue("PG_5V", pg_5v);
    currentData->addValue("PG_HV", pg_hv);
	return currentData;
}

int RCU_Systemstatus::getI2CAddr()
{
	// NOT USED AS THIS IS NOT AN I2C CONNECTED BUS
	return 0;
}

SensorType RCU_Systemstatus::getSensorType()
{
	return SensorType::RCU_SYS_INFO;
}
