#include "Roach2_Actuators_Actuator_HV.h"

Actuator_HV::Actuator_HV() : Actuator()
{
    std::cout << "[Actuator|HV] Initializing" << std::endl;

#ifndef LOCAL_DEV
	// Unexport GPIO pins
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "201", 3); // GPIO HV left RCU pin
		res0 = write(fd_0, "203", 3); // GPIO HV right RCU pin
		close(fd_0);
	}

	// Export GPIO pin
	fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "201", 3); // GPIO HV left RCU pin
		res0 = write(fd_0, "203", 3); // GPIO HV right RCU pin
		close(fd_0);

		// Control direction: set to output
		int fd_1 = open("/sys/class/gpio/gpio201/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio203/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Open the devices
		this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
		this->fd_hv_right = open("/sys/class/gpio/gpio203/value", O_WRONLY);
	}
#endif
}

Actuator_HV::~Actuator_HV()
{
#ifndef LOCAL_DEV
	close(this->fd_hv_left);
	close(this->fd_hv_right);

	// Unexport GPIO pins
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "201", 3); // GPIO HV left RCU pin
		res0 = write(fd_0, "203", 3); // GPIO HV right RCU pin
		close(fd_0);
	}
#endif
}

int Actuator_HV::getActutator_type()
{
	return ACTUATOR_LIST::HV_GEN;
}

void Actuator_HV::enable()
{
#ifndef LOCAL_DEV
	int buffer = 49;

	// Write "1" to output
	write(this->fd_hv_left, "1", 1);
	write(this->fd_hv_right, "1", 1);

	// Reopen device to reset write pointer
 	close(this->fd_hv_left);
	close(this->fd_hv_right);
	this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
	this->fd_hv_right = open("/sys/class/gpio/gpio203/value", O_WRONLY);
#endif
}

void Actuator_HV::disable()
{
#ifndef LOCAL_DEV
	int buffer = 48;

	// Write "1" to output
	write(this->fd_hv_left, "0", 1);
	write(this->fd_hv_right, "0", 1);

	// Reopen device to reset write pointer
	close(this->fd_hv_left);
	close(this->fd_hv_right);
	this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
	this->fd_hv_right = open("/sys/class/gpio/gpio203/value", O_WRONLY);
#endif
}
