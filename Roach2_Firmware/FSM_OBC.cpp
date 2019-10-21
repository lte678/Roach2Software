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
	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_RCU::IDLE;
	this->lastState = -1;

	// Init comm links
	this->debugLink = new UART(); // Will open UART port, must be connected afterwards from PC
	this->debugLink->addEventHandler(this);

	// Database system
	//this->data = new Database();

	// Sensor
	ARM_Systeminfo *info = new ARM_Systeminfo();
	info->init();
	info->update();

	// Add all of them to sensor array (this is possible even that Sensor itself is abstract)
	this->sensors = {info};
	this->numberSensors = 1;

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	while (1) {
		// Receive UART link data
		this->debugLink->receive();

		// Request sensor reading from all connected sensors

		// FSM control
		this->run();

		
	}
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
			case (int)FSM_STATES_OBC::IDLE:
				// Switch RCU to state STANDBY
			break;
			case (int)FSM_STATES_OBC::EXPERIMENT:
				// Switch RCU to state DRIVE_FORWARD
			break;
		}
	} else {
		// No state change, only execute contionous tasks
		switch (this->currentState) {
			case (int)FSM_STATES_OBC::IDLE:
			break;
			case (int)FSM_STATES_OBC::EXPERIMENT:

			break;
		}
	}
}

void FSM_OBC::triggerActuators(void)
{
}

/**
 * @brief Handler for events from the debug UART link
 * @param message: binary message received
 * @param msg_length: message length (number of bytes)
*/
void FSM_OBC::packageReceivedUART(uint64_t message, int msg_length)
{
	Data_simple* send_data;

	// Parse commands
	uint32_t cmd = ((message & 0xFFFFFFFF00000000) >> 32);
	uint32_t para = (message & 0x00000000FFFFFFFF);
	parse_command_type res = CommandParser::parse(cmd);

	// React on commands
	// Operational commands
	switch ((int)res.op) {
		case (int)COMMANDS_OPERATIONAL::obc_check_alive:
			// Check alive command => send command back + 1 in parameter section
			send_data = new Data_simple(cmd, 1);
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_OPERATIONAL::obc_restart_rover:
			// Restart rover
			// Confirm operation and restart
			send_data = new Data_simple(cmd, 1);
			this->debugLink->sendData(send_data, 1);
		break;
	}

	// Debug commands
	switch ((int)res.debug) {
		case (int)COMMANDS_DEBUG::obc_drive_forward:
			// OBC command rover to drive forward
		break;
		case (int)COMMANDS_DEBUG::obc_drive_stop:
			// OBC command rover to stop driving
		break;
		case (int)COMMANDS_DEBUG::obc_next_state:
			// OBC change state, only IDLE to EXPERIMENT possible
			if (this->currentState == (int)FSM_STATES_OBC::IDLE) {
				this->currentState = (int)FSM_STATES_OBC::EXPERIMENT;
			}
			// Send new state number back
			send_data = new Data_simple(cmd, this->currentState);
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_DEBUG::obc_prev_state:
			// OBC change state, only EXPERIMENT to IDLE possible
			if (this->currentState == (int)FSM_STATES_OBC::EXPERIMENT) {
				this->currentState = (int)FSM_STATES_OBC::IDLE;
			}
			// Send new state number back
			send_data = new Data_simple(cmd, this->currentState);
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_DEBUG::obc_rcu_off:
			// Switch RCU/rover off
		break;
		case (int)COMMANDS_DEBUG::obc_rcu_on:
			// Switch RCU/rover on
		break;
		case (int)COMMANDS_DEBUG::obc_read_sensor:
			{
				// Read the sensor with the given sensor id
				Data* data = this->readSensor(para);

				if (data == nullptr) {
					send_data = new Data_simple(cmd, -1);
					this->debugLink->sendData(send_data, 1);
				}
				else {
					this->debugLink->sendData(data, 1);
				}
			}
		break;
		case (int)COMMANDS_DEBUG::obc_sensor_acq_off:
			// Switch sensor acquisiton off
		break;
		case (int)COMMANDS_DEBUG::obc_sensor_acq_on:
			// Switch sensor acquisition on
		break;
	}

	// Sim commands
	switch ((int)res.sim) {
		case ((int)COMMANDS_SIM::obc_lo_event) :
			break;
		case (int)COMMANDS_SIM::obc_sods_event:
			// Switch to experiment
			this->currentState = (int)FSM_STATES_OBC::EXPERIMENT;
			break;
		case (int)COMMANDS_SIM::obc_soe_event:
			break;
		case (int)COMMANDS_SIM::obc_touch_down_event:
			// Power off system
			system("poweroff"); // Shutdown system
			break;
	}
}

void FSM_OBC::rocketSignalReceived(int signal_source)
{
	int i = 0;
}

void FSM_OBC::packageReceivedRexus(uint64_t message, int msg_length)
{
	int i = 0;
}

void FSM_OBC::packageReceivedEthernet()
{
	int i = 0;
}
