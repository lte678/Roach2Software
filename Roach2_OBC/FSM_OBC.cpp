/* @file FSM_OBC.cpp
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "FSM_OBC.h"
//#include <wiringPi.h>

// Ignore infinite loop in static analysis
#pragma clang diagnostic ignored "-Wmissing-noreturn"

FSM_OBC::FSM_OBC()
{
	// init
	std::cout << "[OBC Firmware] Starting." << std::endl;
	sensor_ids.push_back(SensorType::IMU); // sensors: IMU, ARM Info, Temp75B
	sensor_ids.push_back(SensorType::SYS_INFO);
    sensor_ids.push_back(SensorType::TEMP_SENSOR);
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
	std::cout << "[OBC Firmware] Initialization complete" << std::endl;
}


FSM_OBC::~FSM_OBC()
{
}

void FSM_OBC::run() {
    int counter_sensor_downlink = 0;

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    while (true) {
        // Receive UART link data
        Data_simple* data = debugLink->getData();
        if (data != nullptr) {
            packageReceivedUART(data->convert_to_serial()[0], data->convert_to_serial_array_length());
        }

        //  --- FSM control ---
        // Ehemalige run funktion
        stateMachine();

        // Update system time first
        time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

        // Send sensor updates on downlink with 5Hz
        if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count() > 0.2) {
            startTime = startTime + std::chrono::high_resolution_clock::duration(std::chrono::milliseconds (200));
            counter_sensor_downlink = 0;

            Data_super* send_data[1];

            // Send RCU connection status
            if (this->eth_client->isConnected()) {
                send_data[0] = new Data_simple(0x0B, 1);
                debugLink->sendData(send_data, 1);
            }
            else {
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
        usleep(10000); // Main loop running with 100kHz
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
        enableGoPro->enable();

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
        enableGoPro->disable();
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

    if (lastRCUState != currentRCUState) {
        // State change -> send through downlink
        //TODO: Let the RCU send this message through the ethernet connection and pass it along
        Data_super* data_send[1];
        data_send[0] = new Data_simple(0x0D, (unsigned int)currentRCUState);
        debugLink->sendData(data_send, 1);
        // State change. Generate debug message
        std::cout << "[OBC Firmware] State change. RCU: ";
        switch(currentRCUState) {
            case FSM_STATES_RCU::IDLE:
                std::cout << "Idle" << std::endl;
                break;
            case FSM_STATES_RCU::STANDBY:
                std::cout << "Standby" << std::endl;
                break;
            case FSM_STATES_RCU::DRIVE_FORWARD:
                std::cout << "Drive Forward" << std::endl;
                break;
        }
    }
    lastRCUState = currentRCUState; // Update last state variable for change detection
}

void FSM_OBC::packageReceivedRexus(uint64_t message, int msg_length)
{
	this->packageReceivedUART(message, msg_length);
}

void FSM_OBC::packageReceivedEthernet()
{
	int i = 0;
}
