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

	// Load data from corresponding sensor
	bool res;
	do {
		res = this->sensor_manager->getData(&data, (SENSOR_TYPES)sensorId);
	} while (!res);

	// Update sensor
	if (res) {
		return data;
	}
	else {
		return nullptr;
	}
}

void FSM_Controller::initThreads()
{
	// Sensor handling
	this->sensor_manager = new Sensor_Manager(true, false);
	this->sensor_manager->setUpdateRate(10); // 10Hz update rate
	this->sensor_thread = std::thread(&Sensor_Manager::run, this->sensor_manager);

	// UART communication (debug link)
	this->debugLink = new UART(); // Will open UART port, must be connected afterwards from PC
	this->debugLink->addEventHandler(this);
	this->debugLink_thread = std::thread(&UART::run(), this->debugLink);
}
