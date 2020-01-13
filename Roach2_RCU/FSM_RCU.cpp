#include "FSM_RCU.h"

// Ignore infinite loop in static analysis
#pragma clang diagnostic ignored "-Wmissing-noreturn"


FSM_RCU::FSM_RCU()
{
    sensor_ids.push_back(SensorType::TEMP_SENSOR);
    sensor_ids.push_back(SensorType::IMU);
    sensor_ids.push_back(SensorType::SYS_INFO);

	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_RCU::IDLE;
	this->lastState = -1;

	// Init threads
	this->initThreads(PLATFORM::RCU);

	// PWM for engine
	this->pwm = new PWM_PCA985();
	this->pwm->disableLEDs();
	this->pwm->disable();

	// HV generator control
	this->hv = new Actuator_HV();
	this->hv->disable();

	// System start time
    this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

    // sensor logging time
    startTime = std::chrono::high_resolution_clock::now();

    std::cout << "[RCU Firmware] Starting main loop" << std::endl;

	// System main loop
	while (1) {
		// Check if command from OBC incoming
		if (eth_server->isConnected()) {
			while (eth_server->isDataReceived()) {
				packageReceivedEthernet_msg(eth_server->popMessage());
			}
		}

		// FSM control
		run();

		// Sensor downlink ~5 Hz
		sensorDownlink();

		usleep(10000);
	}
}


void FSM_RCU::sensorDownlink() {
    // Update system time first
    time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();
    // Send sensor updates on downlink with 5Hz
    if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count() > 0.2) {
        startTime = startTime + std::chrono::high_resolution_clock::duration(std::chrono::milliseconds(200));

        // Send sensor status update
        for (SensorType sensor : sensor_ids) {
            Data_super *send_data[1];
            // Send sensor status, sensors: IMU, ARM Info, Temp75B
            send_data[0] = readSensor(sensor);

            if (send_data[0] != nullptr) {
                // Downlink over ethernet
                eth_client->send(send_data[0]);
            }
        }
    }
}

FSM_RCU::~FSM_RCU()
{
	
}

void FSM_RCU::triggerActuators()
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

void FSM_RCU::packageReceivedEthernet_msg(const std::string& command)
{
    std::cout << command << std::endl;
	Data_super* msg;

	if (command == "RCU_FSM_STANDBY") {
		this->currentState = (int)FSM_STATES_RCU::STANDBY;
	}
	else if (command == "RCU_FSM_DRIVE_FORWARD") {
		this->currentState = (int)FSM_STATES_RCU::DRIVE_FORWARD;
	}
	else if (command == "RCU_FSM_IDLE") {
		this->currentState = (int)FSM_STATES_RCU::IDLE;
	}
	else if (command == "RCU_DRIVE_FORWARD") {
		this->pwm->enable();
	}
	else if (command == "RCU_STOP_DRIVE_FORWARD") {
		this->pwm->disable();
	}
	else if (command == "rcu_check_alive") {
		msg = new Data_simple("ALIVE");
		//this->eth_client->send(msg);
	}
	else if (command.compare("RCU_SIM_MODE_ENABLE") == 0) {
		// Enable simulation mode
		this->enableSimMode();
	}
	else if (command.compare("RCU_SIM_MODE_DISABLE") == 0) {
		// Disable simulation mode
		this->disableSimMode();
	}
}

void FSM_RCU::simulationModeUpdate()
{
	// Update all actuators: switch HV generator and engines in simulation mode
	if (this->isSimModeEnabled()) {
		this->hv->enableDebugMode();
		this->pwm->enableDebugMode();
	}
	else {
		this->hv->disableDebugMode();
		this->pwm->disableDebugMode();
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
				this->hv->enable();
				usleep(500 * 1000); // 500ms wait
				this->pwm->enable();
			break;
		}
		this->lastState = this->currentState;


		// Inform gs about state change
        eth_client->send(new Data_simple(0x0D, (unsigned int)currentState));
        // State change. Generate debug message
        std::cout << "[RCU Firmware] State change. RCU: ";
        switch(currentState) {
            case (int)FSM_STATES_RCU::IDLE:
                std::cout << "Idle" << std::endl;
                break;
            case (int)FSM_STATES_RCU::STANDBY:
                std::cout << "Standby" << std::endl;
                break;
            case (int)FSM_STATES_RCU::DRIVE_FORWARD:
                std::cout << "Drive Forward" << std::endl;
                break;
        }
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
