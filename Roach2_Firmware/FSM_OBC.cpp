/* @file FSM_OBC.cpp
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "FSM_OBC.h"



FSM_OBC::FSM_OBC()
{
	this->data = new Database();
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

}


FSM_OBC::~FSM_OBC()
{
}
void FSM_OBC::run() {
	// Update system time first
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// FSM control

	// Detect state change, the states are updated by the corresponding ReceiveHandler method
	if (this->lastState != this->currentState) {
		switch (this->currentState) {
			case FSM_STATES_OBC::IDLE:
				// Switch RCU to state STANDBY
				Data *cmd = new Data();
				cmd->setId(0);
				cmd->setTime(this->time);
				cmd->amendColumnName(std::string("Command"));
				cmd->amendValue(4);
			break;
			case FSM_STATES_OBC::EXPERIMENT:
				// Switch RCU to state DRIVE_FORWARD
				Data *cmd = new Data();
				cmd->setId(0);
				cmd->setTime(this->time);
				cmd->amendColumnName(std::string("Command"));
				cmd->amendValue(5);
			break;
		}
	} else {
		// No state change, only execute contionous tasks
		switch (this->currentState) {
			case FSM_STATES_OBC::IDLE:
				this->pollSensors();
				this->saveData();
			break;
			case FSM_STATES_OBC::EXPERIMENT:
				this->pollSensors();
				this->saveData();
			break;
		}
	}
}
