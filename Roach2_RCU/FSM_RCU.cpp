#include "FSM_RCU.h"

#include <memory>

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
	this->pwm->disableLEDs(false);
	this->pwm->disable(false);

	// HV generator control
	this->hv = new Actuator_HV();
	this->hv->disable(false);
}

FSM_RCU::~FSM_RCU()
{

}

void FSM_RCU::sensorDownlink() {
    // Send sensor updates on downlink with 5Hz
    if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count() > 0.2) {
        startTime = startTime + std::chrono::high_resolution_clock::duration(std::chrono::milliseconds(200));

        // Send sensor status update
        for (SensorType sensor : sensor_ids) {
            // Send sensor status, sensors: IMU, ARM Info, Temp75B
            std::unique_ptr<Data_super> send_data(readSensor(sensor));

            if (send_data) {
                // Downlink over ethernet
                eth_client->send(std::move(send_data));
            }
        }
    }
}

void FSM_RCU::triggerActuators()
{
	// Drive forward

}

void FSM_RCU::packageReceivedUART(uint64_t message, int msg_length) {

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

void FSM_RCU::simulationModeUpdate() {
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

void FSM_RCU::packageReceivedEthernet(const std::string& msg) {
    Data_super* binData = EthernetServer::parseBinary(msg);
    // Parameter wird hier nicht genutzt
    std::vector<uint64_t> binArray = binData->convert_to_serial(PLATFORM::UNKNOWN);

    // Gültige Commands haben nur Länge 1
    if(binData->convert_to_serial_array_length() != 1) {
        return;
    }

    uint64_t cmdPacket = binArray[0];

    DATA_TYPE type = CommandParser::packet_type(cmdPacket);
    if(type != DATA_TYPE::COMMAND) {
        // Macht keinen Sinn, wir arbeiten nicht mit Sensor packets oder sonstige
        // Lieber streng sein
        return;
    }

    // Bearbeite das Packet nicht, wenn es nicht den Rover als Ziel hat
    if (CommandParser::get_destination(cmdPacket) != PLATFORM::RCU) {
        return;
    }

    // Alles klar, es ist ein gültiges Command-Packet

    // Parse commands
    uint16_t cmd = CommandParser::get_command(cmdPacket);
    uint32_t para = CommandParser::get_parameter(cmdPacket);
    COMMAND res = CommandParser::parse(cmd);

    Data_super* send_data[1];

    // Process commands
    switch (res) {
        case COMMAND::rcu_hv_on:
            hv->enable(true); // Force change because of debug mode
            break;

        case COMMAND::rcu_hv_off:
            hv->disable(true); // Force change because of debug mode
            break;

        case COMMAND::rcu_drive_enable:
            pwm->enable(true); // Force change because of debug mode
            break;

        case COMMAND::rcu_drive_stop:
            pwm->disable(true); // Force change because of debug mode
            break;

        case COMMAND::rcu_lights_on:
            pwm->enableLEDs(true);
            break;

        case COMMAND::rcu_lights_off:
            pwm->disableLEDs(true);
            break;
    }

}

void FSM_RCU::run() {
    // sensor logging time
    startTime = std::chrono::high_resolution_clock::now();

    std::cout << "[RCU Firmware] Starting main loop" << std::endl;

    // System main loop

    while (1) {
        // Update system time first
        time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

        // Check if command from OBC incoming
        if (eth_server->isConnected()) {
            while (eth_server->isDataReceived()) {
                packageReceivedEthernet(eth_server->popMessage());
            }
        }

        // FSM control (nicht mehr run)
        stateMachine();

        // Sensor downlink ~5 Hz
        sensorDownlink();

        usleep(10000);
    }
}

void FSM_RCU::stateMachine() {
    // Update system time first
    this->time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

    // FSM control
    // Detect state change, the states are updated by the corresponding ReceiveHandler method
    if (this->lastState != this->currentState) {
        switch (this->currentState) {
            case (int)FSM_STATES_RCU::IDLE:
                // Perform selftest
                // Check if all sensors reported at least one data object
                this->pwm->disable(false);
                this->hv->disable(false);
                break;
            case (int)FSM_STATES_RCU::STANDBY:
                this->pwm->disable(false);
                this->pwm->enableLEDs(false);
                this->hv->disable(false);
                break;
            case (int)FSM_STATES_RCU::DRIVE_FORWARD:
                this->hv->enable(false);
                usleep(500 * 1000); // 500ms wait
                this->pwm->enable(false);
                break;
        }
        this->lastState = this->currentState;


        // Inform gs about state change
        std::unique_ptr<Data_super> msg(new Data_simple(0x0D, (unsigned int)currentState));
        eth_client->send(std::move(msg));
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
