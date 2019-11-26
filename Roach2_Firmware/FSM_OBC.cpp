/* @file FSM_OBC.cpp
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "FSM_OBC.h"
//#include <wiringPi.h>

FSM_OBC::FSM_OBC()
{
	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_OBC::IDLE;
	this->lastState = -1;

	// Init tasks running in separate threads (communication, sensors)
	this->initThreads();

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	while (1) {
		// Receive UART link data
		Data_simple* data = this->debugLink->getData();
		if (data != nullptr) {
			this->packageReceivedUART(*(data->convert_to_serial()), data->convert_to_serial_array_length());
		}

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
	Data_super* send_data[1];

	// Parse commands
	uint32_t cmd = ((message & 0xFFFFFFFF00000000) >> 32);
	uint32_t para = (message & 0x00000000FFFFFFFF);
	parse_command_type res = CommandParser::parse(cmd);

	// React on commands
	// Operational commands
	switch ((int)res.op) {
		case (int)COMMANDS_OPERATIONAL::obc_check_alive:
			// Check alive command => send command back + 1 in parameter section
			send_data[0] = new Data_simple(cmd, 1);
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_OPERATIONAL::obc_restart_rover:
			// Restart rover
			// Confirm operation and restart
			send_data[0] = new Data_simple(cmd, 1);
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
			send_data[0] = new Data_simple(cmd, this->currentState);
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_DEBUG::obc_prev_state:
			// OBC change state, only EXPERIMENT to IDLE possible
			if (this->currentState == (int)FSM_STATES_OBC::EXPERIMENT) {
				this->currentState = (int)FSM_STATES_OBC::IDLE;
			}
			// Send new state number back
			send_data[0] = new Data_simple(cmd, this->currentState);
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
				Data_super* data[1];
				data[0] = this->readSensor(para);

				if (data == nullptr) {
					send_data[0] = new Data_simple(cmd, -1);
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
