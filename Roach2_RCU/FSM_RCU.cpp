#include "FSM_RCU.h"

FSM_RCU::FSM_RCU()
{
	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_RCU::IDLE;
	this->lastState = -1;

	// Init comm links
	this->debug_link = new UART(); // Will open UART port, must be connected afterwards from PC
	this->debug_link->addEventHandler(this);

	// Database system
	//this->data = new Database();

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	while (1) {
		// Receive UART link data
		this->debug_link->receive();
		
		// Request sensor reading from all connected sensors

		// FSM control
		this->run();
	}
}


FSM_RCU::~FSM_RCU()
{
	delete this->debug_link;
}

void FSM_RCU::triggerActuators(void) 
{
	// Drive forward

}

void FSM_RCU::packageReceivedUART(uint64_t message, int msg_length)
{
	Data_simple* send_data;

	// Parse commands
	uint32_t cmd = ((message & 0xFFFFFFFF00000000) >> 32);
	uint32_t para = (message & 0x00000000FFFFFFFF);
	parse_command_type res = CommandParser::parse(cmd);

	// React on command
	switch ((int)res.op) {
		// RCU alive check
		case (int)COMMANDS_OPERATIONAL::rcu_check_alive:
			// Check alive => RCU is alive
			// Send command back and 1 in parameter section
			send_data = new Data_simple(cmd, 1);
			this->debug_link->sendData(send_data, 1);
		break;

		// LO event
		case (int)COMMANDS_OPERATIONAL::rcu_lo_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::LO);
			
			// Send new state back
			send_data = new Data_simple(cmd, this->currentState);
			this->debug_link->sendData(send_data, 1);
		break;

		// Read sensors
		case (int)COMMANDS_OPERATIONAL::rcu_read_sensor:
		break;

		// SODS event
		case (int)COMMANDS_OPERATIONAL::rcu_sods_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SODS);

			// Send new state back
			send_data = new Data_simple(cmd, this->currentState);
			this->debug_link->sendData(send_data, 1);
		break;

		// SOE event
		case (int)COMMANDS_OPERATIONAL::rcu_soe_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SOE);
			// Send new state back
			send_data = new Data_simple(cmd, this->currentState);
			this->debug_link->sendData(send_data, 1);
		break;
	}

	// Debug commands
	switch ((int)res.debug) {
		case (int)COMMANDS_DEBUG::rcu_drive_forward:
		break;

		case (int)COMMANDS_DEBUG::rcu_drive_stop:
		break;

		case (int)COMMANDS_DEBUG::rcu_hv_off:
		break;

		case (int)COMMANDS_DEBUG::rcu_hv_on:
		break;

		// Switch to next state
		case (int)COMMANDS_DEBUG::rcu_next_state:
			if (this->currentState == (int)FSM_STATES_RCU::IDLE) {
				this->currentState = (int)FSM_STATES_RCU::STANDBY;
			} else if (this->currentState == (int)FSM_STATES_RCU::STANDBY) {
				this->currentState = (int)FSM_STATES_RCU::DRIVE_FORWARD;
			}

			// Send current state back
			send_data = new Data_simple(cmd, (uint32_t)this->currentState);
			this->debug_link->sendData(send_data, 1);
		break;

		// Switch to revious state
		case (int)COMMANDS_DEBUG::rcu_prev_state:
			if (this->currentState == (int)FSM_STATES_RCU::STANDBY) {
				this->currentState = (int)FSM_STATES_RCU::IDLE;
			} else if (this->currentState == (int)FSM_STATES_RCU::DRIVE_FORWARD) {
				this->currentState = (int)FSM_STATES_RCU::STANDBY;
			}

			// Send current state back
			send_data = new Data_simple(cmd, (uint32_t)this->currentState);
			this->debug_link->sendData(send_data, 1);
		break;

		case (int)COMMANDS_DEBUG::rcu_sensor_acq_off:
		break;

		case (int)COMMANDS_DEBUG::rcu_sensor_acq_on:
		break;
	}
}

void FSM_RCU::rocketSignalReceived(int signal_source)
{
	if (signal_source == (int)REXUS_SIGNALS::LO) {
		// Not relevant for FSM control
	}
	else if (signal_source == (int)REXUS_SIGNALS::SODS) {
		if (this->currentState == (int)FSM_STATES_RCU::IDLE) {
			// Go into standby
			this->currentState = (int)FSM_STATES_RCU::STANDBY;
		}
	}
	else if (signal_source == (int)REXUS_SIGNALS::SOE) {
		if (this->currentState == (int)FSM_STATES_RCU::STANDBY) {
			// Start driving
			this->currentState = (int)FSM_STATES_RCU::DRIVE_FORWARD;
		}
		else if (this->currentState == (int)FSM_STATES_RCU::DRIVE_FORWARD) {
			// Stop driving
			this->currentState = (int)FSM_STATES_RCU::STANDBY;
		}
	}
}

void FSM_RCU::packageReceivedRexus(uint64_t message, int msg_length)
{
	this->packageReceivedUART(message, msg_length); // Relay to UART receiver function
}

void FSM_RCU::packageReceivedEthernet()
{
}

void FSM_RCU::run(void) 
{
	// Update system time first
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// FSM control
	// Detect state change, the states are updated by the corresponding ReceiveHandler method
	if (this->lastState != this->currentState) {
		switch (this->currentState) {
			case (int)FSM_STATES_RCU::IDLE:
				// Perform selftest
				// Check if all sensors reported at least one data object
				
			break;
			case (int)FSM_STATES_RCU::STANDBY:

			break;
			case (int)FSM_STATES_RCU::DRIVE_FORWARD:

			break;
		}
		this->lastState = this->currentState;
	}
	else {
		switch (this->currentState) {
			case (int)FSM_STATES_RCU::IDLE:
				// Check if selftest done
			break;
			case (int)FSM_STATES_RCU::STANDBY:
				this->pollSensors();
			break;
			case (int)FSM_STATES_RCU::DRIVE_FORWARD:
				this->pollSensors();
				this->triggerActuators();
			break;
		}
	}
}
