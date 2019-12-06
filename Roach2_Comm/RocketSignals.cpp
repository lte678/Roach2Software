/* @file ReceiveHandler.cpp
 * @date 20.06.2019
 * @brief Interface for all classes to implement which want to be informed about received data from the communication links
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#include "RocketSignals.h"

RocketSignals *instance_rocket_signals; // Only use this here!! This is a solution to use the wiringPi interrupt function which cannot handle class instances

/**
 * @brief Checks the GPIO pin values of SODS, SOE and LO. Then compares them with previous values to detect changes. 
 * @return bool true if at least on GPIO pin has changed, otherwise false
*/
bool RocketSignals::checkGpio()
{
	bool changed = false;
	bool new_sods = false;
	bool new_soe = false;
	bool new_lo = false;
	int buffer = 0;

	// Check GPIO status, 0 = false, 1 = true (or any other value beside 0) (see https://www.kernel.org/doc/html/v4.17/driver-api/gpio/legacy.html#sysfs-interface-for-userspace-optional)
	this->access_limit.lock();

	// Check SODS
	// Note, the "value" is a text (ASCII) literal, so false = 0 = 48, true = 1 = 49
	if (this->fd_sods != -1) {
		read(this->fd_sods, &buffer, 1);
		if (buffer != 48) {
			this->sig_sods_acc++;
		}
	}

	// Check SOE
	if (this->fd_soe != -1) {
		read(this->fd_soe, &buffer, 1);
		if (buffer != 48) {
			this->sig_soe_acc++;
		}
	}

	// Check LO
	if (this->fd_lo != -1) {
		read(this->fd_lo, &buffer, 1);
		if (buffer != 48) {
			this->sig_lo_acc++;
		}
	}

	if (this->sig_counter < this->max_sig_counter) {
		this->sig_counter++;
	}
	else {
		// Evaluate accumulator registers
		double sig_lo = this->sig_lo_acc / (double)this->max_sig_counter;
		double sig_soe = this->sig_soe_acc / (double)this->max_sig_counter;
		double sig_sods = this->sig_sods_acc / (double)this->max_sig_counter;

		// Check if "1"
		if (sig_lo > 0.9) {
			new_lo = true;
		}
		if (sig_soe > 0.9) {
			new_soe = true;
		}
		if (sig_sods > 0.9) {
			new_sods = true;
		}

		// Check against last status
		if (this->sods != new_sods) {
			changed = true;
		}
		if (this->soe != new_soe) {
			changed = true;
		}
		if (this->lo != new_lo) {
			changed = true;
		}

		// Write back
		this->soe = new_soe;
		this->sods = new_sods;
		this->lo = new_lo;

		// Reset counter
		this->sig_counter = 0;
		this->sig_lo_acc = 0;
		this->sig_sods_acc = 0;
		this->sig_soe_acc = 0;
	}

	// Unlock
	this->access_limit.unlock();

	// We need to read the same line always, so close the file handlers at the end to reset the read function
	close(this->fd_sods);
	close(this->fd_soe);
	close(this->fd_lo);
	this->fd_sods = open("/sys/class/gpio/gpio67/value", O_RDONLY);
	this->fd_soe = open("/sys/class/gpio/gpio66/value", O_RDONLY);
	this->fd_lo = open("/sys/class/gpio/gpio65/value", O_RDONLY);

	return changed;
}

/**
 * @brief Inits the GPIO pins which are connected to the RXSM signals SODS, SOE and LO.
 * This GPIO uses sysfs interface from Linux Kernel directly. 
 */
void RocketSignals::initGpio()
{
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
	this->lo = false;
	this->sods = false;
	this->soe = false;
}

/*
 * @brief Constructor
 **/
RocketSignals::RocketSignals()
{
	this->sig_counter = 0;
	this->sig_lo_acc = 0;
	this->sig_sods_acc = 0;
	this->sig_soe_acc = 0;
}

/*
 * @brief Tells which connection is used if the abstract baseclass Connection is used
 * @return int connection/link type index (UART, RS422/RXSM, Ethernet, GPIO interrupts)
**/
int RocketSignals::whichConnection() {
	return ROCKET_SIGNALS;
}

/**
 * @brief Main loop for rocket signal detection, should be called from within a new thread.
*/
void RocketSignals::run()
{
	running.store(true);
	this->initGpio();

	while (running.load()) {
		bool res = this->checkGpio();
		// Only store if something changed
		if (res) {
			this->changed.store(res);
			sleep(1);
		}
		usleep(10); // 10us sleep => effective capture frequency upto 100kHz (depending on Linux)
	}
}

/**
 * @brief Stops thread execution
*/
void RocketSignals::stop()
{
	this->running.store(false);
}

/**
 * @brief Returns true if a signal was detected on any of the pins (= pin logic level changed)
*/
bool RocketSignals::signalChanged()
{
	bool buffer = this->changed.load();
	this->changed.store(false);
	return buffer;
}

/**
 * @brief Returns the rocket signals (three element array: 0=SODS, 1=SOE, 2=LO)
 * @return boolean array of length 3 with the current signal logic levels
*/
bool* RocketSignals::getRocketSignals()
{
	static bool result[3];

	// Get lock
	this->access_limit.lock();

	result[0] = this->sods;
	result[1] = this->soe;
	result[2] = this->lo;

	// Release lock
	this->access_limit.unlock();

	return result;
}
