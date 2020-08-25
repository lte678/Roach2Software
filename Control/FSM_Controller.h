/* @file FSM_Controller.h
 * @date 20.06.2019
 * @brief Abstract FSM controller class for usage in OBC and RCU firmware
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef FSM_CONTROLLER_HEADER
#define FSM_CONTROLLER_HEADER

#include "../DataStore/data.h"
#include <thread>
#include "../Comm/ReceiveHandler.h"
#include "../Sensor/Sensor.h"
#include "../Comm/EthernetClient.h"
#include "../Comm/EthernetServer.h"
#include "../Sensor/RocketSignals.h"
#include "../Sensor/LaserDist_VL530LX.h"
#include "../Comm/UART.h"
#include "Defines.h"
#include "Sensor_Manager.h"

enum class FSM_STATES_RCU {
    INVALID = -1,
	IDLE = 0,
	STANDBY = 1,
	DRIVE_FORWARD = 2 
};

enum class FSM_STATES_OBC {
    INVALID = -1,
	IDLE = 0,
	EXPERIMENT = 1,
};

class FSM_Controller
{
private:
	bool isSimMode = false;
protected:
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
	void triggerActuators();
	void saveData();
    std::unique_ptr<Data> readSensor(SensorType sensorId);
	virtual void simulationModeUpdate() = 0;
	void enableSimMode();
	void disableSimMode();
	void initThreads(PLATFORM target);
public:
	virtual void run() = 0;
    bool isSimModeEnabled();
    bool isEthDownlink();
    bool isEthUplink();
};

#endif
