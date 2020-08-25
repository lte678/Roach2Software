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
#include <iostream>

#include "../Control/FSM_Controller.h"
#include "../Comm/ReceiveHandler.h"
#include "../DataStore/data.h"
#include "../DataStore/Data_super.h"
#include "../DataStore/Data_simple.h"
#include "../Control/CommandParser.h"
#include "../Comm/UART.h"
#include "../Sensor/ARM_Systeminfo.h"
#include "../Sensor/Temp.h"
#include "../Sensor/IMU.h"
#include "../Actuators/Rover.h"
#include "../Actuators/GoPro.h"


class FSM_OBC :
	public FSM_Controller,
	public ReceiveHandler
{
private:
	FSM_STATES_RCU currentRCUState;
	Actuator_GoPro* enableGoPro;
	Actuator_Rover* enableRoverPower;
    std::vector<SensorType> sensor_ids;
    std::chrono::high_resolution_clock::time_point startTime;
    bool prevSOE, prevLO, prevSODS;
    FSM_STATES_OBC currentState;
    FSM_STATES_OBC lastState;
    FSM_STATES_RCU currentRcuState;
    FSM_STATES_RCU lastRcuState;

	void sendRXSMSignalUpdate_Downlink();
    void sensorDownlink();
public:
	FSM_OBC();
	~FSM_OBC();
	void run() override;
	void triggerActuators();
	void packageReceivedUART(uint64_t message, int msg_length) override;
	void stateMachine();
	void packageReceivedRexus(uint64_t message, int msg_length) override;
	void packageReceivedEthernet(const std::string& msg) override;
	void rocketSignalReceived(int) override {};
	void simulationModeUpdate() override;
};

#endif

