/* @file ReceiveHandler.cpp
 * @date 20.06.2019
 * @brief Interface for all classes to implement which want to be informed about received data from the communication links
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#include "RocketSignals.h"


/*
 * @brief Constructor
 **/
RocketSignals::RocketSignals(float updateFreq) : Sensor(updateFreq) {
	sig_lo_acc = 0;
	sig_sods_acc = 0;
	sig_soe_acc = 0;
}

/**
 * @brief Inits the GPIO pins which are connected to the RXSM signals SODS, SOE and LO.
 */
void RocketSignals::init()
{
    std::cout << "[Sensor|RX Signals] Initializing" << std::endl;

    // Create GPIOs
    loPin = new GPIODevice(65, GPIODevice::INPUT);
    soePin = new GPIODevice(66, GPIODevice::INPUT);
    sodsPin = new GPIODevice(67, GPIODevice::INPUT);
    if(!(loPin->isValid() && soePin->isValid() && sodsPin->isValid())) {
        std::cout << "[Sensor|RX Signals] Critical: GPIOs invalid!" << std::endl;
    }

    // Init pin values
    lo = false;
    sods = false;
    soe = false;
}


/**
 * @brief Checks the GPIO pin values of SODS, SOE and LO. Then compares them with previous values to detect changes.
 * Changes are logged in changed
*/
void RocketSignals::update()
{
	bool new_sods = false;
	bool new_soe = false;
	bool new_lo = false;

	// Check GPIO status, 0 = false, 1 = true (or any other value beside 0) (see https://www.kernel.org/doc/html/v4.17/driver-api/gpio/legacy.html#sysfs-interface-for-userspace-optional)
	this->access_limit.lock();

	int buffer = 0;
	for (int i = 0; i < max_sig_counter; i++) {
		if(loPin->read()) {
		    sig_lo_acc++;
		}
        if(soePin->read()) {
            sig_soe_acc++;
        }
        if(sodsPin->read()) {
            sig_sods_acc++;
        }
		nanosleep(reinterpret_cast<const timespec *>(sig_sample_interval), nullptr);
	}

	// Evaluate accumulator registers
	double sig_lo = (double)sig_lo_acc / (double)max_sig_counter;
	double sig_soe = (double)sig_soe_acc / (double)max_sig_counter;
	double sig_sods = (double)sig_sods_acc / (double)max_sig_counter;

	// Check if "1"
	if (sig_lo > 0.85) {
		new_lo = true;
	}
	if (sig_soe > 0.85) {
		new_soe = true;
	}
	if (sig_sods > 0.85) {
		new_sods = true;
	}

	// Check against last status
	if (sods != new_sods) {
		changed = true;
	}
	if (soe != new_soe) {
		changed = true;
	}
	if (lo != new_lo) {
		changed = true;
	}

	// Write back
	soe = new_soe;
	sods = new_sods;
	lo = new_lo;

	// Reset counters
	sig_lo_acc = 0;
	sig_sods_acc = 0;
	sig_soe_acc = 0;

	// Unlock
	access_limit.unlock();
}

/**
 * @brief Returns true if a signal was detected on any of the pins (= pin logic level changed)
*/
bool RocketSignals::signalChanged()
{
	bool buffer = changed;
	changed = false;
	return buffer;
}

/**
 * @brief Returns state of Lift-Off signal
*/
bool RocketSignals::getLO() {
    bool state;

    access_limit.lock();
    state = lo;
    access_limit.unlock();

    return state;
}

/**
 * @brief Returns state of Start-of-Experiment signal
*/
bool RocketSignals::getSOE() {
    bool state;

    access_limit.lock();
    state = soe;
    access_limit.unlock();

    return state;
}

/**
 * @brief Returns state of SODs signal
*/
bool RocketSignals::getSODS() {
    bool state;

    access_limit.lock();
    state = sods;
    access_limit.unlock();

    return state;
}

SensorType RocketSignals::getSensorType() {
    return SensorType::RX_SIGNALS;
}

int RocketSignals::getI2CAddr() {
    return 0;
}

std::unique_ptr<Data> RocketSignals::getData() {
    access_limit.lock();

    std::unique_ptr<Data> data_ptr(new Data());
    data_ptr->setId((int)SensorType::RX_SIGNALS);
    data_ptr->addValue("LO", lo);
    data_ptr->addValue("SOE", soe);
    data_ptr->addValue("SODS", sods);

    access_limit.unlock();

    return data_ptr;
}
