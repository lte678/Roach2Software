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
#include "../Roach2_Comm/EthernetClient.h"
#include "../Roach2_Comm/EthernetServer.h"
#include "../Roach2_Sensor/RocketSignals.h"
#include "../Roach2_Comm/UART.h"
#include "Sensor_Manager.h"

enum class FSM_STATES_RCU {
	IDLE = 0,
	STANDBY = 1,
	DRIVE_FORWARD = 2 
};

enum class FSM_STATES_OBC {
	IDLE = 0,
	EXPERIMENT = 1,
};

enum class REBOOT_TARGET {
	RCU = 0,
	OBC = 1
};

class FSM_Controller
{
private:
	bool isSimMode = false;
protected:
	int currentState;
	int lastState;
	int time;
	Sensor_Manager* sensor_manager;
	RocketSignals* rocket_signals;
	EthernetClient* eth_client;
	EthernetServer* eth_server;
	std::thread eth_client_thread;
	std::thread eth_server_thread;
	std::thread debugLink_thread;
	std::thread sensor_thread;
	UART* debugLink;
	int numberSensors;
	int numberActuators;
	int numberData;
	Data_super* logs;
protected:
	void triggerActuators(void);
	void saveData(void);
	Data* readSensor(SensorType sensorId);
	void initThreads(REBOOT_TARGET target);
	virtual void simulationModeUpdate() = 0;
	void enableSimMode();
	void disableSimMode();
	bool isSimModeEnabled();
public:
	virtual void run() = 0;
};

#endif
