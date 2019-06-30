/* @file FSM_Controller.h
 * @date 20.06.2019
 * @brief Abstract FSM controller class for usage in OBC and RCU firmware
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef FSM_CONTROLLER_HEADER
#define FSM_CONTROLLER_HEADER

#include "../Roach2_DataStore/database.h"
#include "../Roach2_Comm/ReceiveHandler.h"

enum FSM_STATES_RCU {
	IDLE,
	STANDBY,
	DRIVE_FORWARD
};

enum FSM_STATES_OBC {
	IDLE,
	EXPERIMENT
};

enum REBOOT_TARGET {
	RCU,
	OBC
};

class FSM_Controller : ReceiveHandler
{
protected:
	int currentState;
	int lastState;
	int time;
	Sensor* sensors;
	Actuator* actuators;
	Database* data;
	Connection* connections;
	bool isSimMode;
	Data* logs;
protected:
	void pollSensors(void);
	void triggerActuators(void);
	void saveData(void);
	void enableSimMode(void);
	void disableSimMode(void);
public:
	virtual void run(void) = 0;
	void packageReceivedEthernet();
	void packageReceivedRexus();
	void rocketSignalReceived();
	void packageReceivedUART();
};

#endif
