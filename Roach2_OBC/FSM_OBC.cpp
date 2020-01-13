/* @file FSM_OBC.cpp
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "FSM_OBC.h"

// Ignore infinite loop in static analysis
#pragma clang diagnostic ignored "-Wmissing-noreturn"

FSM_OBC::FSM_OBC()
{
	// init
	std::cout << "[OBC Firmware] Starting." << std::endl;
	sensor_ids.push_back(SensorType::IMU); // sensors: IMU, ARM Info, Temp75B
	sensor_ids.push_back(SensorType::SYS_INFO);
    sensor_ids.push_back(SensorType::TEMP_SENSOR);

	// Init state and trigger selftest through run method
	this->currentState = (int)FSM_STATES_OBC::IDLE;
	this->lastState = -1;
    // Disable automically sending sensor and status information downstream

	// Rover control
	this->enableRoverPower = new Actuator_Rover();
	this->enableRoverPower->enable(false);

	// Init tasks running in separate threads (communication, sensors)
	this->initThreads(PLATFORM::OBC);

	// GoPro control
	this->enableGoPro = new Actuator_GoPro();

	// System start time
	this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

	// System main loop
	std::cout << "[OBC Firmware] Initialization complete" << std::endl;
}


FSM_OBC::~FSM_OBC()
{
}

void FSM_OBC::run() {
    startTime = std::chrono::high_resolution_clock::now();

    while (true) {
        // Receive UART link data
        Data_simple *data = debugLink->getData();
        if (data != nullptr) {
            packageReceivedUART(data->convert_to_serial(PLATFORM::GS)[0], data->convert_to_serial_array_length());
            delete data;
        }

        //  --- FSM control ---
        // Ehemalige run funktion
        stateMachine();

        // Handle ethernet messages from rover
        while (eth_server->isDataReceived()) {
            std::string msg = eth_server->popMessage();
            //std::cout << "Ethernet message: " << msg << std::endl;
            Data_super *binMsg[1];
            binMsg[0] = EthernetServer::parseBinary(msg);
            if (binMsg[0]) {
                // Forward binary data to ground station
                debugLink->sendData(binMsg, 1);
            }

        }

        // Send sensor updates on downlink with 5Hz
        sensorDownlink();

        usleep(10000); // Main loop running with 100 Hz
    }
}

void FSM_OBC::triggerActuators()
{
}

void FSM_OBC::sensorDownlink() {
    // Update system time first
    time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();
    // Send sensor updates on downlink with 5Hz
    if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count() > 0.2) {
        startTime = startTime + std::chrono::high_resolution_clock::duration(std::chrono::milliseconds(200));

        Data_super *send_data[1];

        // Send RCU connection status
        if (this->eth_client->isConnected()) {
            send_data[0] = new Data_simple(0x0B, 1);
            debugLink->sendData(send_data, 1);
        } else {
            send_data[0] = new Data_simple(0x0B, 0);
            debugLink->sendData(send_data, 1);
        }

        // Send sensor status update
        for (SensorType sensor : sensor_ids) {
            // Send sensor status, sensors: IMU, ARM Info, Temp75B
            send_data[0] = readSensor(sensor);

            if (send_data[0] != nullptr) {
                debugLink->sendData(send_data, 1);
            }
        }

        // Send RXSM signal status
        sendRXSMSignalUpdate_Downlink();
    }
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
			this->enableRoverPower->disable(false);
			usleep(1000 * 1000); // 1s warten
			this->enableRoverPower->enable(false);
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
			this->enableRoverPower->enable(true);
		break;

		case (int)COMMANDS_DEBUG::obc_rcu_on:
			// Switch RCU/rover on
			this->enableRoverPower->disable(true);
		break;

		case (int)COMMANDS_DEBUG::obc_read_sensor:
			{
				// Read the sensor with the given sensor id
                Data_super* data[1];
				for(SensorType sensor : sensor_ids) {
				    if((uint32_t)sensor == para) {
                        data[0] = readSensor(sensor);
				    }
				}

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
            ;
		break;

		case (int)COMMANDS_DEBUG::obc_sensor_acq_on:
			// Switch sensor acquisition on
            ;
		break;

		// Simulation control
        case (int)COMMANDS_DEBUG::obc_sim_control:
            if (para == 1) {
                // enable simulation mode
                this->enableSimMode();
            }
            else {
                // disable simulation mode
                this->disableSimMode();
            }
            break;
	}

	// Sim commands
	switch ((int)res.sim) {
		case ((int)COMMANDS_SIM::obc_lo_event) :
			//this->rocketSignalReceived((int)REXUS_SIGNALS::LO);
			break;
		case (int)COMMANDS_SIM::obc_sods_event:
			//this->rocketSignalReceived((int)REXUS_SIGNALS::SODS);
			break;
		case (int)COMMANDS_SIM::obc_soe_event:
			//this->rocketSignalReceived((int)REXUS_SIGNALS::SOE);
			break;
		case (int)COMMANDS_SIM::obc_touch_down_event:
			// Power off system
			system("poweroff"); // Shutdown system
			break;
	}
}

void FSM_OBC::sendRXSMSignalUpdate_Downlink() {
    uint32_t signals_binary = 0;

    // Convert to integer for downlink
    if (rocket_signals->getSODS()) {
        signals_binary += 1;
    }
    if (rocket_signals->getSOE()) {
        signals_binary += 2;
    }
    if (rocket_signals->getLO()) {
        signals_binary += 4;
    }

	// Notify connected UART debug console
	Data_super* data[1];
	data[0] = new Data_simple(0x0A, signals_binary);
	debugLink->sendData(data, 1);

}

void FSM_OBC::stateMachine()
{
	// Message to RCU
	Data_super* msg;

	bool lo = rocket_signals->getLO();
	bool sods = rocket_signals->getSODS();
	bool soe = rocket_signals->getSOE();

	// ------ State change OBC IDLE -> EXPERIMENT ------
	if (currentState == (int)FSM_STATES_OBC::IDLE && sods) {
		currentState = (int)FSM_STATES_OBC::EXPERIMENT; // Switch to experiment state
        enableGoPro->enable(false);

        // Switch RCU from IDLE to STANDBY
        if (currentRCUState != FSM_STATES_RCU::STANDBY) {
            msg = new Data_simple("RCU_FSM_STANDBY");
            eth_client->send(msg);
            currentRCUState = FSM_STATES_RCU::STANDBY; // Note: Message to RCU is in run() method
        }
	}

    // ------ State change OBC EXPERIMENT -> IDLE ------
	if(currentState == (int)FSM_STATES_OBC::EXPERIMENT && !sods) {
	    currentState = (int)FSM_STATES_OBC::IDLE; // Switch back to idle
        enableGoPro->disable(false);
	}


    // ------ State change RCU STANDBY -> DRIVE_FORWARD ------
	if (currentState == (int)FSM_STATES_OBC::EXPERIMENT && soe) {
        // Switch RCU from STANDBY to DRIVE_FORWARD if SOE given
        if (currentRCUState == FSM_STATES_RCU::STANDBY) {
            // Send drive forward command
            msg = new Data_simple("RCU_FSM_DRIVE_FORWARD");
            eth_client->send(msg);
            currentRCUState = FSM_STATES_RCU::DRIVE_FORWARD;
        }
    }

    // ------ State change RCU DRIVE_FORWARD -> STANDBY ------
    if (!soe) {
        if (currentRCUState != FSM_STATES_RCU::STANDBY) {
            // Send standby command
            msg = new Data_simple("RCU_FSM_STANDBY");
            eth_client->send(msg);
        }
        currentRCUState = FSM_STATES_RCU::STANDBY;
    }

    // Detect state change
    if (lastState != currentState) {
        // State change -> send through downlink
        Data_super* data_send[1];
        data_send[0] = new Data_simple(0x0C, (unsigned int)currentState);
        debugLink->sendData(data_send, 1);
        // Write debug message
        std::cout << "[OBC Firmware] State change. OBC: ";
        switch(currentState) {
            case (int)FSM_STATES_OBC::IDLE:
                std::cout << "Idle" << std::endl;
                break;
            case (int)FSM_STATES_OBC::EXPERIMENT:
                std::cout << "Experiment" << std::endl;
                break;
        }
    }
    lastState = currentState; // Update last state variable for change detection
}

void FSM_OBC::packageReceivedRexus(uint64_t message, int msg_length)
{
	this->packageReceivedUART(message, msg_length);
}

void FSM_OBC::packageReceivedEthernet()
{
	int i = 0;
}

/**
 * @brief Called if a change to the simulation mode properties occurs
*/
void FSM_OBC::simulationModeUpdate() {
    Data_simple* msg;

    if (this->isSimModeEnabled()) {
        // Update all actuators: on OBC side nothing to be done

        // Send update to RCU
        msg = new Data_simple("RCU_SIM_MODE_ENABLE");
        this->eth_client->send(msg);
    }
    else {
        // Disable sim mode

        // Send update to RCU
        msg = new Data_simple("RCU_SIM_MODE_DISABLE");
        this->eth_client->send(msg);
    }

}