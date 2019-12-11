#include "FSM_RCU.h"

FSM_RCU::FSM_RCU()
{
	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_RCU::IDLE;
	this->lastState = -1;

	// Init threads
	this->initThreads(REBOOT_TARGET::RCU);

	// PWM for engine
	this->pwm = new PWM_PCA985();
	this->pwm->disableLEDs();
	this->pwm->disable();

	// HV generator control
	this->hv = new Actuator_HV();
	this->hv->disable();

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	while (1) {
		
		// Check if command from OBC incoming
		if (this->eth_server->isConnected()) {
			if (this->eth_server->isDataReceived()) {
				std::vector<std::string> msgs = this->eth_server->getReceivedValues();
				for (int i = 0; i < msgs.size(); i++) {
					std::string message = msgs[i];
					this->packageReceivedEthernet_msg(message);
				}
			}
		}

		// FSM control
		this->run();

		usleep(100);
	}
}


FSM_RCU::~FSM_RCU()
{
	
}

void FSM_RCU::triggerActuators(void) 
{
	// Drive forward

}

void FSM_RCU::packageReceivedUART(uint64_t message, int msg_length)
{
	// Parse commands
	uint32_t cmd = ((message & 0xFFFFFFFF00000000) >> 32);
	uint32_t para = (message & 0x00000000FFFFFFFF);
	parse_command_type res = CommandParser::parse(cmd);

	// React on command
	switch ((int)res.op) {
		// RCU alive check
		case (int)COMMANDS_OPERATIONAL::rcu_check_alive:

		break;

		// LO event
		case (int)COMMANDS_OPERATIONAL::rcu_lo_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::LO);
		break;

		// Read sensors
		case (int)COMMANDS_OPERATIONAL::rcu_read_sensor:
		break;

		// SODS event
		case (int)COMMANDS_OPERATIONAL::rcu_sods_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SODS);
		break;

		// SOE event
		case (int)COMMANDS_OPERATIONAL::rcu_soe_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SOE);
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

		break;

		// Switch to revious state
		case (int)COMMANDS_DEBUG::rcu_prev_state:
			if (this->currentState == (int)FSM_STATES_RCU::STANDBY) {
				this->currentState = (int)FSM_STATES_RCU::IDLE;
			} else if (this->currentState == (int)FSM_STATES_RCU::DRIVE_FORWARD) {
				this->currentState = (int)FSM_STATES_RCU::STANDBY;
			}
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

void FSM_RCU::packageReceivedEthernet_msg(std::string command)
{
	Data_super* msg;

	if (command.compare("RCU_FSM_STANDBY") == 0) {
		this->currentState = (int)FSM_STATES_RCU::STANDBY;
	}
	else if (command.compare("RCU_FSM_DRIVE_FORWARD") == 0) {
		this->currentState = (int)FSM_STATES_RCU::DRIVE_FORWARD;
	}
	else if (command.compare("RCU_FSM_IDLE") == 0) {
		this->currentState = (int)FSM_STATES_RCU::IDLE;
	}
	else if (command.compare("RCU_DRIVE_FORWARD") == 0) {
		this->pwm->enable();
	}
	else if (command.compare("RCU_STOP_DRIVE_FORWARD") == 0) {
		this->pwm->disable();
	}
	else if (command.compare("rcu_check_alive") == 0) {
		msg = new Data_simple("ALIVE");
		this->eth_client->send(msg);
	}
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
				this->pwm->disable();
				this->hv->disable();
			break;
			case (int)FSM_STATES_RCU::STANDBY:
				this->pwm->disable();
				this->pwm->enableLEDs();
				this->hv->disable();
			break;
			case (int)FSM_STATES_RCU::DRIVE_FORWARD:
				this->pwm->enable();
				this->hv->enable();
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
				
			break;
			case (int)FSM_STATES_RCU::DRIVE_FORWARD:
				
			break;
		}
	}
}
