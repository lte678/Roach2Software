/* @file FSM_Controller.h
 * @date 20.06.2019
 * @brief Abstract FSM controller class for usage in OBC and RCU firmware
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef FSM_CONTROLLER_HEADER
#define FSM_CONTROLLER_HEADER

#include "../Roach2_DataStore/data.h"
#include <thread>
#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_Sensor/Roach2_Sensor_Sensor.h"
#include "Sensor_Manager.h"

enum class FSM_STATES_RCU {
	IDLE = 0,
	STANDBY = 1,
	DRIVE_FORWARD = 2 
};

enum class FSM_STATES_OBC {
	IDLE = 0,
	EXPERIMENT = 1
};

enum class REBOOT_TARGET {
	RCU = 0,
	OBC = 1
};

class FSM_Controller
{
protected:
	int currentState;
	int lastState;
	int time;
	Sensor_Manager* sensor_manager;
	std::thread sensor_thread;
	int numberSensors;
	int numberActuators;
	int numberData;
	//Actuator* actuators;
	//Database* data;
	bool isSimMode;
	Data_super* logs;
protected:
	void triggerActuators(void);
	void saveData(void);
	void enableSimMode(void);
	void disableSimMode(void);
	Data* readSensor(int sensorId);
public:
	virtual void run(void) = 0;
};

#endif
