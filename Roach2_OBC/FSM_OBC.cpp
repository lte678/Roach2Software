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
            packageReceivedEthernet(msg);
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

        // Send RCU connection status
        if (this->eth_client->isConnected()) {
            std::unique_ptr<Data_super> send_data(new Data_simple(0x0B, 1));
            debugLink->sendData(std::move(send_data));
        } else {
            std::unique_ptr<Data_super> send_data(new Data_simple(0x0B, 0));
            debugLink->sendData(std::move(send_data));
        }

        // Send sensor status update
        for (SensorType sensor : sensor_ids) {
            // Send sensor status, sensors: IMU, ARM Info, Temp75B
            std::unique_ptr<Data_super> sensorData(readSensor(sensor));
            if (sensorData) {
                debugLink->sendData(std::move(sensorData));
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
    DATA_TYPE type = CommandParser::packet_type(message);
    if(type != DATA_TYPE::COMMAND) {
        // Macht keinen Sinn, wir arbeiten nicht mit Sensor packets oder sonstige
        // Lieber streng sein
        return;
    }

    // Leite die Daten an den Rover weiter, wenn dieser als Ziel angegeben ist
    if (CommandParser::get_destination(message) == PLATFORM::RCU) {
        std::unique_ptr<Data_Raw> toForward(new Data_Raw());
        toForward->addElement(message);
        eth_client->send(std::move(toForward));
    }

    // Es handelt sich um ein gültiges Command Packet

	// Parse commands
	uint16_t cmd = CommandParser::get_command(message);
	uint32_t para = CommandParser::get_parameter(message);
	parse_command_type res = CommandParser::parse(cmd);

	// React on commands
	// Operational commands
	switch ((int)res.op) {
		case (int)COMMANDS_OPERATIONAL::obc_check_alive:
			// Check alive command => send command back + 1 in parameter section
			if (eth_client->isConnected()) {
				// Rover connected
                std::unique_ptr<Data_super> send_data(new Data_simple(cmd, 2));
                debugLink->sendData(std::move(send_data));
			}
			else {
				// No rover connected
                std::unique_ptr<Data_super> send_data(new Data_simple(cmd, 1));
                debugLink->sendData(std::move(send_data));
			}
		break;

		case (int)COMMANDS_OPERATIONAL::obc_restart_rover:
			// Restart rover
			enableRoverPower->disable(false);
			usleep(1000 * 1000); // 1s warten
			enableRoverPower->enable(false);
			// Confirm operation and restart
			std::unique_ptr<Data_super> send_data(new Data_simple(cmd, 1));
			debugLink->sendData(std::move(send_data));
		break;
	}

	Data_super* msg;

	// Debug commands
    std::unique_ptr<Data_super> send_data;
	switch ((int)res.debug) {
		case (int)COMMANDS_DEBUG::obc_drive_forward:
			// OBC command rover to drive forward
		break;

		case (int)COMMANDS_DEBUG::obc_drive_stop:
			// OBC command rover to stop driving
		break;

		case (int)COMMANDS_DEBUG::obc_next_state:
			// OBC change state, only IDLE to EXPERIMENT possible
			if (currentState == (int)FSM_STATES_OBC::IDLE) {
				currentState = (int)FSM_STATES_OBC::EXPERIMENT;
			}
			// Send new state number back
            send_data = std::unique_ptr<Data_super>(new Data_simple(cmd, currentState));
			debugLink->sendData(std::move(send_data));
		break;

		case (int)COMMANDS_DEBUG::obc_prev_state:
			// OBC change state, only EXPERIMENT to IDLE possible
			if (currentState == (int)FSM_STATES_OBC::EXPERIMENT) {
				currentState = (int)FSM_STATES_OBC::IDLE;
			}
			// Send new state number back
            send_data = std::unique_ptr<Data_super>(new Data_simple(cmd, currentState));
			debugLink->sendData(std::move(send_data));
		break;

		case (int)COMMANDS_DEBUG::obc_rcu_off:
			// Switch RCU/rover off
			enableRoverPower->enable(true);
		break;

		case (int)COMMANDS_DEBUG::obc_rcu_on:
			// Switch RCU/rover on
			enableRoverPower->disable(true);
		break;

		case (int)COMMANDS_DEBUG::obc_read_sensor:
			{
				// Read the sensor with the given sensor id
				for(SensorType sensor : sensor_ids) {
				    if((uint32_t)sensor == para) {
                        send_data = readSensor(sensor);
				    }
				}

				// Send error back instead
				if (!send_data) {
					send_data = std::unique_ptr<Data_super>(new Data_simple(cmd, -1));
				}
                debugLink->sendData(std::move(send_data));
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
	std::unique_ptr<Data_super> data( new Data_simple((uint32_t)COMMANDS_OPERATIONAL::obc_rocket_signal_status, signals_binary));
	debugLink->sendData(std::move(data));

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
            eth_client->send("RCU_FSM_STANDBY");
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
            std::unique_ptr<Data_simple> msg(new Data_simple("RCU_FSM_DRIVE_FORWARD"));
            eth_client->send(std::move(msg));
            currentRCUState = FSM_STATES_RCU::DRIVE_FORWARD;
        }
    }

    // ------ State change RCU DRIVE_FORWARD -> STANDBY ------
    if (!soe) {
        if (currentRCUState != FSM_STATES_RCU::STANDBY) {
            // Send standby command
            eth_client->send("RCU_FSM_STANDBY");
        }
        currentRCUState = FSM_STATES_RCU::STANDBY;
    }

    // Detect state change
    if (lastState != currentState) {
        // State change -> send through downlink
        std::unique_ptr<Data_super> data_send(new Data_simple(0x0C, (unsigned int)currentState));
        debugLink->sendData(std::move(data_send));
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

/**
 * @brief Called if a change to the simulation mode properties occurs
*/
void FSM_OBC::simulationModeUpdate() {
    if (this->isSimModeEnabled()) {
        // Update all actuators: on OBC side nothing to be done

        // Send update to RCU
        this->eth_client->send("RCU_SIM_MODE_ENABLE");
    }
    else {
        // Disable sim mode

        // Send update to RCU
        this->eth_client->send("RCU_SIM_MODE_DISABLE");
    }
}

/**
 * @brief Handle with ethernet data from rover
*/
void FSM_OBC::packageReceivedEthernet(const std::string &msg) {
    std::unique_ptr<Data_super> binMsg(EthernetServer::parseBinary(msg));
    if (binMsg) {
        // Forward binary data to ground station
        debugLink->sendData(std::move(binMsg));
    }
}
