#include "OBC_Systemstatus.h"

OBC_Systemstatus::OBC_Systemstatus(float updateFreq, EthernetClient* client, EthernetServer* server) : Sensor(updateFreq)
{
	this->eth_client = client;
	this->eth_server = server;
}

void OBC_Systemstatus::init()
{
	// Unexport pins (if they were already exported)
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "201", 3); // Power good rover
		int fd_2 = write(fd_0, "203", 3); // Power good GoPros/camera
		close(fd_0);
	}

	// Export pins
	fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "201", 3); // Power good rover
		int fd_2 = write(fd_0, "203", 3); // Power good GoPros/camera
		close(fd_0);
	}

	// Set pins to input
	fd_0 = open("sys/class/gpio/gpio201/direction", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "in", 5);
		close(fd_0);
	}
	fd_0 = open("sys/class/gpio/gpio203/direction", O_WRONLY);
	if (fd_0 != -1) {
		int fd_1 = write(fd_0, "in", 5);
		close(fd_0);
	}

}

void OBC_Systemstatus::update()
{
	int buffer = 0;

	pg_rover = 0;
	pg_cam = 0;

	// Load sensor values:
	// Rover power good
	int fd_0 = open("sys/class/gpio/gpio201/value", O_RDONLY);
	if (fd_0 != -1) {
		read(fd_0, &buffer, 1); // ASCII 48=0, 49=1
		if (buffer == 49) {
			pg_rover = 1;
		}
		close(fd_0);
	}

	buffer = 0; // Reset buffer

	// OBC cameras (GoPro) power good
	fd_0 = open("sys/class/gpio/gpio203/value", O_RDONLY);
	if (fd_0 != -1) {
		read(fd_0, &buffer, 1);
		if (buffer == '1') {
			pg_cam = 1;
		}
		close(fd_0);
	}

	// Load connection status to RCU
	obc_uplink_rcu = this->eth_client->isConnected() ? 1 : 0;
	obc_downlink_rcu = this->eth_server->isConnected() ? 1 : 0;
}

std::unique_ptr<Data> OBC_Systemstatus::getData()
{
    std::unique_ptr<Data> currentData(new Data());
    currentData->addValue("OBC_UP_RCU", obc_uplink_rcu);
    currentData->addValue("OBC_DOWN_RCU", obc_downlink_rcu);
    currentData->addValue("PG_ROVER", pg_rover);
    currentData->addValue("PG_CAM", pg_cam);
	return currentData;
}

int OBC_Systemstatus::getI2CAddr()
{
	// NOT USED AS THIS IS NOT AN I2C CONNECTED SENSOR
	return 0;
}

SensorType OBC_Systemstatus::getSensorType()
{
	return SensorType::OBC_SYS_INFO;
}
