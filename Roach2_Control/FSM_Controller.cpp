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
