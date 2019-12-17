/* @file FSM_OBC.h
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef HEADER_FSM_OBC
#define HEADER_FSM_OBC

#include <chrono>
#include <string>

#include "../Roach2_Control/FSM_Controller.h"
#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_super.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "../Roach2_Control/CommandParser.h"
#include "../Roach2_Comm/UART.h"
#include "../Roach2_Sensor/ARM_Systeminfo.h"
#include "../Roach2_Sensor/Temp.h"
#include "../Roach2_Sensor/imu_neu.h"
#include "../Roach2_Actuators/Roach2_Actuators_Actuator_Rover.h"
#include "../Roach2_Actuators/Roach2_Actuators_Actuartor_GoPro.h"


class FSM_OBC :
	public FSM_Controller,
	public ReceiveHandler
{
private:
	FSM_STATES_RCU currentRCUState;
	Actuator_GoPro* enableGoPro;
	Actuator_Rover* enableRoverPower;
	bool enableDownstream;
	void sendRXSMSignalUpdate_Downlink(uint32_t signals_binary);
public:
	FSM_OBC();
	~FSM_OBC();
	void run(void);
	void triggerActuators(void);
	void packageReceivedUART(uint64_t message, int msg_length);
	void rocketSignalReceived(int signal_source);
	void packageReceivedRexus(uint64_t message, int msg_length);
	void packageReceivedEthernet();
	
};

#endif

