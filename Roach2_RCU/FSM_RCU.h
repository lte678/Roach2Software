#ifndef HEADER_FSM_RCU
#define HEADER_FSM_RCU

#include "../Roach2_Control/FSM_Controller.h"
#include <chrono>

class FSM_RCU :
	public FSM_Controller
{
public:
	FSM_RCU();
	~FSM_RCU();
	void run(void);
	void triggerActuators(void);
};

#endif
