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
	// init
	int sensor_ids[] = {0, 1, 10}; // sensors: IMU, ARM Info, Temp75B
	std::vector<Data*> sensor_data;

	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_OBC::IDLE;
	this->lastState = -1;
	this->enableDownstream = false; // Disable automically sending sensor and status information downstream

	// Rover control
	this->enableRoverPower = new Actuator_Rover();
	this->enableRoverPower->disable();
	usleep(10000);
	this->enableRoverPower->enable();

	// Init tasks running in separate threads (communication, sensors)
	this->initThreads(REBOOT_TARGET::OBC);

	// GoPro control
	this->enableGoPro = new Actuator_GoPro();

	// We are not in simulation mode by default
	this->isSimMode = false;

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	int counter_sensor_downlink = 0;
	while (1) {

		// Receive UART link data
		Data_simple* data = this->debugLink->getData();
		if (data != nullptr) {
			this->packageReceivedUART(*(data->convert_to_serial()), data->convert_to_serial_array_length());
		}

		// FSM control
		this->run();

		// RXSM signals
		if (this->rocket_signals->signalChanged()) {
			this->rocketSignalReceived((int)REXUS_SIGNALS::ALL);
		}

		// Send sensor updates on downlink with 5Hz
		if (counter_sensor_downlink == 19999) {
			counter_sensor_downlink = 0;

			Data_super* send_data[1];

			// Send RCU connection status
			if (this->eth_client->isConnected()) {
				send_data[0] = new Data_simple(0x0B, 1);
				this->debugLink->sendData(send_data, 1);
			}
			else {
				send_data[0] = new Data_simple(0x0B, 0);
				this->debugLink->sendData(send_data, 1);
			}

			// Send sensor status update
			for (int k = 0; k < 3; k++) {
				// Send sensor status, sensors: IMU, ARM Info, Temp75B

				send_data[0] = this->readSensor(sensor_ids[k]);

				if (send_data[0] != nullptr) {
					this->debugLink->sendData(send_data, 1);
				}
			}

			// Send RXSM signal status
			uint32_t signals_binary = 0;
			bool* signal_levels = new bool[3];
			signal_levels = this->rocket_signals->getRocketSignals(); // 0=SODS, 1=SOE, 2=LO

			// Convert to integer for downlink
			if (signal_levels[0]) {
				signals_binary += 1;
			}
			if (signal_levels[1]) {
				signals_binary += 2;
			}
			if (signal_levels[2]) {
				signals_binary += 4;
			}
			this->sendRXSMSignalUpdate_Downlink(signals_binary);

		}
		else {
			counter_sensor_downlink++;
		}

		usleep(100); // Main loop running with 100kHz
	}
}


FSM_OBC::~FSM_OBC()
{
}
void FSM_OBC::run() {
	// Message to RCU
	Data_super* msg;

	// Update system time first
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// FSM control

	// Detect state change, the states are updated by the corresponding ReceiveHandler method
	if (this->lastState != this->currentState) {

		// State change -> send through downlink
		Data_super* data_send[1];
		data_send[0] = new Data_simple(0x0C, this->currentState);
		this->debugLink->sendData(data_send, 1);

		switch (this->currentState) {
			case (int)FSM_STATES_OBC::IDLE:
				this->enableGoPro->disable();

				// Power on RCU
			break;
			case (int)FSM_STATES_OBC::EXPERIMENT:
				this->enableGoPro->enable();

				msg = new Data_simple("RCU_FSM_STANDBY");
				this->eth_client->send(msg);
			break;
		}
		this->lastState = this->currentState; // Update last state variable for change detection
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

void FSM_OBC::triggerActuators()
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
			if (this->eth_client->isConnected()) {
				// Rover connected
				send_data[0] = new Data_simple(cmd, 2);
			}
			else {
				// No rover connected
				send_data[0] = new Data_simple(cmd, 1);
			}
			this->debugLink->sendData(send_data, 1);
		break;
		case (int)COMMANDS_OPERATIONAL::obc_restart_rover:
			// Restart rover
			this->enableRoverPower->disable();
			usleep(10000);
			this->enableRoverPower->enable();
			// Confirm operation and restart
			send_data[0] = new Data_simple(cmd, 1);
			this->debugLink->sendData(send_data, 1);
		break;
	}

	Data_super* msg;

	// Debug commands
	switch ((int)res.debug) {
		case (int)COMMANDS_DEBUG::obc_drive_forward:
			// OBC command rover to drive forward
			msg = new Data_simple("RCU_DRIVE_FORWARD");
			this->eth_client->send(msg);
		break;
		case (int)COMMANDS_DEBUG::obc_drive_stop:
			// OBC command rover to stop driving
			msg = new Data_simple("RCU_STOP_DRIVE_FORWARD");
			this->eth_client->send(msg);
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
			this->enableRoverPower->enable();
		break;
		case (int)COMMANDS_DEBUG::obc_rcu_on:
			// Switch RCU/rover on
			this->enableRoverPower->disable();
		break;
		case (int)COMMANDS_DEBUG::obc_read_sensor:
			{
				// Read the sensor with the given sensor id
				Data_super* data[1];
				data[0] = this->readSensor(para);

				if (data[0] == nullptr) {
					send_data[0] = new Data_simple(cmd, -1);
					this->debugLink->sendData(send_data, 1);
				}
				else {
					this->debugLink->sendData(data, 1);
				}
			}
		break;
		// Control sensor and system status information downstream
		case (int)COMMANDS_DEBUG::obc_sensor_acq_off:
			// Switch sensor acquisiton off
			this->enableDownstream = false;
		break;
		case (int)COMMANDS_DEBUG::obc_sensor_acq_on:
			// Switch sensor acquisition on
			this->enableDownstream = true;
		break;
	}

	// Sim commands
	switch ((int)res.sim) {
		case ((int)COMMANDS_SIM::obc_lo_event) :
			this->rocketSignalReceived((int)REXUS_SIGNALS::LO);
			break;
		case (int)COMMANDS_SIM::obc_sods_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SODS);
			break;
		case (int)COMMANDS_SIM::obc_soe_event:
			this->rocketSignalReceived((int)REXUS_SIGNALS::SOE);
			break;
		case (int)COMMANDS_SIM::obc_touch_down_event:
			// Power off system
			system("poweroff"); // Shutdown system
			break;
	}
}

void FSM_OBC::sendRXSMSignalUpdate_Downlink(uint32_t signals_binary) {
	
	// Notify connected UART debug console
	Data_super* data[1];
	data[0] = new Data_simple(0x0A, signals_binary);
	this->debugLink->sendData(data, 1);

}

void FSM_OBC::rocketSignalReceived(int signal_source)
{
	uint32_t signals_binary = 0;

	// Message to RCU
	Data_super* msg;

	// Load the signal levels
	// If set to ALL, load the current signal sources from the rocket signals detection class otherwise use the given signal sources
	bool* signal_levels = new bool[3];
	signal_levels[0] = false;
	signal_levels[1] = false;
	signal_levels[2] = false;

	// Handling of signals (ALL is the real case, the others are used for simulation)
	switch (signal_source)
	{
		case (int)REXUS_SIGNALS::LO:
			signal_levels[2] = true;
			signals_binary = 4;
		break;

		case (int)REXUS_SIGNALS::SODS:
			signal_levels[0] = true;
			signals_binary = 1;
		break;

		case (int)REXUS_SIGNALS::SOE:
			signal_levels[1] = true;
			signals_binary = 2;
		break;

		case (int)REXUS_SIGNALS::ALL:
			signal_levels = this->rocket_signals->getRocketSignals(); // 0=SODS, 1=SOE, 2=LO

			// Convert to integer for downlink
			if (signal_levels[0]) {
				signals_binary += 1;
			}
			if (signal_levels[1]) {
				signals_binary += 2;
			}
			if (signal_levels[2]) {
				signals_binary += 4;
			}
		break;
	}
	// Downlink
	this->sendRXSMSignalUpdate_Downlink(signals_binary);
	
	// In air
	// Follow normal FSM, only one state change possible
	if (this->currentState == (int)FSM_STATES_OBC::IDLE && signal_levels[0]) {
		this->currentState = (int)FSM_STATES_OBC::EXPERIMENT; // Switch to experiment state

		msg = new Data_simple("RCU_FSM_STANDBY");
		this->eth_client->send(msg);

		// Switch RCU from IDLE to STANDBY
		if (this->currentRCUState != FSM_STATES_RCU::STANDBY) {
			this->currentRCUState = FSM_STATES_RCU::STANDBY; // Note: Message to RCU is in run() method
		}
	}
		
	if (this->currentState = (int)FSM_STATES_OBC::EXPERIMENT) {
		// Switch RCU from STANDBY to DRIVE_FORWARD if SOE given
		if (this->currentRCUState == FSM_STATES_RCU::STANDBY && signal_levels[1]) {
			if (this->currentRCUState != FSM_STATES_RCU::DRIVE_FORWARD) {
				// Send drive forward command
				msg = new Data_simple("RCU_FSM_DRIVE_FORWARD");
				this->eth_client->send(msg);
			}
			this->currentRCUState = FSM_STATES_RCU::DRIVE_FORWARD;
		}

		// Stop GoPro only once!
		if (!signal_levels[0]) {
			this->enableGoPro->disableGoPro();
		}

		if (this->currentState == (int)FSM_STATES_OBC::EXPERIMENT && !signal_levels[0]) {
			this->currentState = (int)FSM_STATES_OBC::IDLE;
		}

		// Switch RCU from DRIVE FORWARD to STANDBY if SOE was deasserted
		if (this->currentRCUState == FSM_STATES_RCU::DRIVE_FORWARD && !signal_levels[1]) {
			if (this->currentRCUState != FSM_STATES_RCU::STANDBY) {
				// Send standby command
				msg = new Data_simple("RCU_FSM_STANDBY");
				this->eth_client->send(msg);
			}
			this->currentRCUState = FSM_STATES_RCU::STANDBY;
		}
	}
}

void FSM_OBC::packageReceivedRexus(uint64_t message, int msg_length)
{
	this->packageReceivedUART(message, msg_length);
}

void FSM_OBC::packageReceivedEthernet()
{
	int i = 0;
}
