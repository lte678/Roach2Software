#ifndef HEADER_FSM_RCU
#define HEADER_FSM_RCU

#include "../Roach2_Control/FSM_Controller.h"
#include "../Roach2_DataStore/data_simple.h"
#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_Control/CommandParser.h"
#include "../Roach2_Comm/UART.h"
#include <chrono>

class FSM_RCU :
	public FSM_Controller,
	public ReceiveHandler
{
private:
	UART* debug_link;
public:
	FSM_RCU();
	~FSM_RCU();
	void run(void);
	void triggerActuators(void);
	void packageReceivedUART(uint64_t message, int msg_length);
	void rocketSignalReceived(int signal_source);
	void packageReceivedRexus(uint64_t message, int msg_length);
	void packageReceivedEthernet();
};

#endif
