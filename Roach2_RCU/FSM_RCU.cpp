#include "FSM_RCU.h"



FSM_RCU::FSM_RCU()
{
	this->data = new Database();
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

}


FSM_RCU::~FSM_RCU()
{
}

void FSM_RCU::triggerActuators(void) 
{
	// Drive forward

}

void FSM_RCU::run(void) 
{
	// Update system time first
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// FSM control

	// Detect state change, the states are updated by the corresponding ReceiveHandler method
	if (this->lastState != this->currentState) {
		switch (this->currentState) {
			case FSM_STATES_RCU::IDLE:
				// Perform selftest
				// Check if all sensors reported at least one data object
				
			break;
			case FSM_STATES_RCU::STANDBY:

			break;
			case FSM_STATES_RCU::DRIVE_FORWARD:

			break;
		}
	}
	else {
		switch (this->currentState) {
			case FSM_STATES_RCU::IDLE:
				// Check if selftest done
			break;
			case FSM_STATES_RCU::STANDBY:
				this->pollSensors();
			break;
			case FSM_STATES_RCU::DRIVE_FORWARD:
				this->pollSensors();
				this->triggerActuators();
			break;
		}
	}
}
