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
	sig_counter = 0;
	sig_lo_acc = 0;
	sig_sods_acc = 0;
	sig_soe_acc = 0;
}

/**
 * @brief Inits the GPIO pins which are connected to the RXSM signals SODS, SOE and LO.
 * This GPIO uses sysfs interface from Linux Kernel directly.
 */
void RocketSignals::init()
{
    std::cout << "[Sensor|RX Signals] Initializing" << std::endl;

    // Sunix sysfs interface init
    int fd_0 = open("/sys/class/gpio/export", O_WRONLY);
    if (fd_0 != -1) {
        int res0 = write(fd_0, "67", 2); // SODS pin
        int res1 = write(fd_0, "66", 2); // SOE pin
        int res2 = write(fd_0, "65", 2); // LO pin
        close(fd_0);

        // Control direction: set to input
        int fd_1 = open("/sys/class/gpio/gpio67/direction", O_WRONLY);
        if (fd_1 != -1) {
            int res = write(fd_1, "in", 2);
            close(fd_1);
        }
        else {
            // Error!
        }
        int fd_2 = open("/sys/class/gpio/gpio66/direction", O_WRONLY);
        if (fd_2 != -1) {
            int res = write(fd_2, "in", 2);
            close(fd_2);
        }
        else {
            // Error!
        }

        int fd_3 = open("/sys/class/gpio/gpio65/direction", O_WRONLY);
        if (fd_3 != -1) {
            int res = write(fd_3, "in", 2);
            close(fd_3);
        }
        else {
            // Error!
        }

        // Open GPIOs for later usage, read only
        this->fd_sods = open("/sys/class/gpio/gpio67/value", O_RDONLY);
        this->fd_soe = open("/sys/class/gpio/gpio66/value", O_RDONLY);
        this->fd_lo = open("/sys/class/gpio/gpio65/value", O_RDONLY);
    }
    else {
        // Error!
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
		read(fd_lo, &buffer, 1);
		lseek(fd_lo, 0, SEEK_SET);
		if (buffer == '1') {
			sig_lo_acc++;
		}
		read(fd_soe, &buffer, 1);
		lseek(fd_soe, 0, SEEK_SET);
		if (buffer == '1') {
			sig_soe_acc++;
		}
		read(fd_sods, &buffer, 1);
		lseek(fd_sods, 0, SEEK_SET);
		if (buffer == '1') {
			this->sig_sods_acc++;
		}
		nanosleep(reinterpret_cast<const timespec *>(sig_sample_interval), nullptr);
	}

	// Evaluate accumulator registers
	double sig_lo = sig_lo_acc / (double)max_sig_counter;
	double sig_soe = sig_soe_acc / (double)max_sig_counter;
	double sig_sods = sig_sods_acc / (double)max_sig_counter;

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

	// Reset counter
	sig_counter = 0;
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

Data *RocketSignals::getData() {
    access_limit.lock();

    Data* data_ptr = new Data();
    data_ptr->setId((int)SensorType::RX_SIGNALS);
    data_ptr->addValue("LO", lo);
    data_ptr->addValue("SOE", soe);
    data_ptr->addValue("SODS", sods);

    access_limit.unlock();

    return data_ptr;
}
