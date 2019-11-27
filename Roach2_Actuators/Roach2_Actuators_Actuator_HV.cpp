#include "Roach2_Actuators_Actuator_HV.h"

Actuator_HV::Actuator_HV() : Actuator()
{
	// Export GPIO pin
	int fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "201", 2); // GPIO HV left RCU pin
		res0 = write(fd_0, "203", 2); // GPIO HV right RCU pin
		close(fd_0);

		// Control direction: set to output
		int fd_1 = open("/sys/class/gpio/gpio201/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 2);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio203/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 2);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Open the devices
		this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
		this->fd_hv_right = open("/sys/class/gpio/gpio203/value", O_WRONLY);
	}
}

Actuator_HV::~Actuator_HV()
{
	close(this->fd_hv_left);
	close(this->fd_hv_right);

	// Unexport GPIO pins
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "201", 2); // GPIO HV left RCU pin
		res0 = write(fd_0, "203", 2); // GPIO HV right RCU pin
		close(fd_0);
	}
}

int Actuator_HV::getActutator_type()
{
	return ACTUATOR_LIST::HV_GEN;
}

void Actuator_HV::enable()
{
	int buffer = 49;

	// Write "1" to output
	write(this->fd_hv_left, &buffer, 1);
	write(this->fd_hv_right, &buffer, 1);

	// Reopen device to reset write pointer
	close(this->fd_hv_left);
	close(this->fd_hv_right);
	this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
	this->fd_hv_left = open("/sys/class/gpio/gpio203/value", O_WRONLY);
}

void Actuator_HV::disable()
{
	int buffer = 48;

	// Write "1" to output
	write(this->fd_hv_left, &buffer, 1);
	write(this->fd_hv_right, &buffer, 1);

	// Reopen device to reset write pointer
	close(this->fd_hv_left);
	close(this->fd_hv_right);
	this->fd_hv_left = open("/sys/class/gpio/gpio201/value", O_WRONLY);
	this->fd_hv_left = open("/sys/class/gpio/gpio203/value", O_WRONLY);
}
