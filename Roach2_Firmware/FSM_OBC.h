/* @file FSM_OBC.h
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef HEADER_FSM_OBC
#define HEADER_FSM_OBC

#include "../Roach2_Control/FSM_Controller.h"
#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "../Roach2_Control/CommandParser.h"
#include "../Roach2_Comm/UART.h"
#include "../Roach2_Sensor/ARM_Systeminfo.h"
#include "../Roach2_Sensor/Temp.h"
#include "../Roach2_Sensor/imu_neu.h"
#include <chrono>

class FSM_OBC :
	public FSM_Controller,
	public ReceiveHandler
{
private:
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

