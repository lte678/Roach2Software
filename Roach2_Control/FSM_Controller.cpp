/* @file FSM_Controller.cpp
 * @date 20.06.2019
 * @brief Abstract FSM controller class for usage in OBC and RCU firmware
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */
#include "FSM_Controller.h"


void FSM_Controller::triggerActuators()
{
}

void FSM_Controller::saveData()
{

}

/*
* @brief Enables simulation mode, this information will be passed to all registered actuators and sensors
*/
void FSM_Controller::enableSimMode(void)
{
	this->isSimMode = true;
	

}

/*
 * @brief Disables simulation mode, this information will be passed to all registered actuators and sensors
 */
void FSM_Controller::disableSimMode(void)
{
	this->isSimMode = false;
}

Data* FSM_Controller::readSensor(int sensorId)
{
	Data* data = nullptr;

	bool res = this->sensor_manager->getData(&data, (SENSOR_TYPES)sensorId);

	// Update sensor
	if (res) {
		return data;
	}
	else {
		return nullptr;
	}
}

void FSM_Controller::initThreads(REBOOT_TARGET target)
{
	// Sensor handling
	if (target == REBOOT_TARGET::OBC) 
	{
		// OBC
		this->sensor_manager = new Sensor_Manager(true, false);
	}
	else 
	{
		// RCU
		this->sensor_manager = new Sensor_Manager(false, true);
	}
	this->sensor_manager->setUpdateRate(10); // 10Hz update rate
	this->sensor_thread = std::thread(&Sensor_Manager::run, this->sensor_manager);
	
	if (target == REBOOT_TARGET::OBC) {
		// Rocket signals from RXSM
		this->rocket_signals = new RocketSignals();
		this->rocket_signals_capture_thread = std::thread(&RocketSignals::run, this->rocket_signals);

		// UART communication (debug link)
		this->debugLink = new UART(); // Will open UART port, must be connected afterwards from PC
		this->debugLink_thread = std::thread(&UART::run, this->debugLink);
	}
	
	// Ethernet links: server
	this->eth_server = new EthernetServer();
	this->eth_server_thread = std::thread(&EthernetServer::run, this->eth_server);

	// Ethernet links: client
	if (target == REBOOT_TARGET::OBC) {
		// OBC connects to RCU
		this->eth_client = new EthernetClient("192.168.100.100");
		this->eth_client_thread = std::thread(&EthernetClient::run, this->eth_client);
	}
	else {
		// RCU connects to OBC
		this->eth_client = new EthernetClient("192.168.100.101");
		this->eth_client_thread = std::thread(&EthernetClient::run, this->eth_client);
	}
}
