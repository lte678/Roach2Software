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

bool FSM_Controller::isSimModeEnabled()
{
	return this->isSimMode;
}

std::unique_ptr<Data> FSM_Controller::readSensor(SensorType sensorId)
{
	std::unique_ptr<Data> data;

	bool res = sensor_manager->getData(data, sensorId);

	// Update sensor
	if (res) {
		return data;
	}
	else {
		return nullptr;
	}
}

void FSM_Controller::initThreads(PLATFORM target)
{
    isSimMode = false;

	if (target == PLATFORM::OBC) {
		// UART communication (debug link)
		debugLink = new UART(target); // Will open UART port, must be connected afterwards from PC
		debugLink_thread = std::thread(&UART::run, debugLink);
	}
	
	// Ethernet links: server
	eth_server = new EthernetServer();
	eth_server_thread = std::thread(&EthernetServer::run, eth_server);

	// Ethernet links: client
	if (target == PLATFORM::OBC) {
		// OBC connects to RCU
        std::cout << "[OBC Firmware] Connecting to RCU" << std::endl;
#ifndef LOCAL_DEV
		eth_client = new EthernetClient("192.168.100.100", target);
#else
        eth_client = new EthernetClient("rcupi", target);
#endif
		eth_client_thread = std::thread(&EthernetClient::run, eth_client);
	}
	else {
		// RCU connects to OBC
        std::cout << "[RCU Firmware] Connecting to OBC" << std::endl;
#ifndef LOCAL_DEV
        eth_client = new EthernetClient("192.168.100.101", target);
#else
        eth_client = new EthernetClient("obcpi", target);
#endif
		eth_client_thread = std::thread(&EthernetClient::run, eth_client);
	}

	// Sensor handling
	if (target == PLATFORM::OBC)
	{
		// OBC
		sensor_manager = new Sensor_Manager();
		rocket_signals = new RocketSignals(50.0f);
		sensor_manager->attachSensor(rocket_signals);
        sensor_manager->attachSensor(new ARM_Systeminfo(10.0f));
        sensor_manager->attachSensor(new TEMP_LM75B(10.0f));
        sensor_manager->attachSensor(new BNO055_IMU(10.0f));
        sensor_manager->attachSensor(new ADC_MCP3428(10.0f));
        sensor_manager->attachSensor(new OBC_Systemstatus(10.0f, this));
	}
	else
	{
		// RCU
		sensor_manager = new Sensor_Manager();
        sensor_manager->attachSensor(new ARM_Systeminfo(10.0f));
        sensor_manager->attachSensor(new TEMP_LM75B(10.0f));
        sensor_manager->attachSensor(new ADC_MCP3428(10.0f));
        sensor_manager->attachSensor(new BNO055_IMU(10.0f));
        sensor_manager->attachSensor(new ROT_AS5601(10.0f));
        sensor_manager->attachSensor(new LaserDist_VL530LX(10.0f, SensorType::LASERDIST_0, 0x52));
        sensor_manager->attachSensor(new LaserDist_VL530LX(10.0f, SensorType::LASERDIST_1, 0x53));
        sensor_manager->attachSensor(new RCU_Systemstatus(10.0f, this));
	}

	sensor_thread = std::thread(&Sensor_Manager::run, sensor_manager);
}

/**
 * @brief Enables the simulation mode. In case it was disabled before, the simulationModeUpdate method is called.
*/
void FSM_Controller::enableSimMode() {
    // Only call simulationModeUpdate if sim mode property changed
    if (!isSimMode) {
        std::cout << "[FSM] Enabling sim mode" << std::endl;
        isSimMode = true;
        simulationModeUpdate();
    }
}

/**
 * @brief Disables the simulation mode. In case it was disabled before, the simulationModeUpdate method is called.
*/
void FSM_Controller::disableSimMode() {
	// Only call simulationModeUpdate if sim mode property changed
	if (isSimMode) {
        std::cout << "[FSM] Disabling sim mode" << std::endl;
        isSimMode = false;
        simulationModeUpdate();
	}
}

bool FSM_Controller::isEthDownlink() {
    return eth_server->isConnected();
}

bool FSM_Controller::isEthUplink() {
    return eth_client->isConnected();
}