#include "Roach2_Actuators_Actuator_Rover.h"

Actuator_Rover::Actuator_Rover()
{
    std::cout << "[Actuator|Rover] Initializing" << std::endl;

#ifndef LOCAL_DEV
	// Unexport GPIO pin
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "6", 2); // GPIO Rover Power enable OBC pin
		close(fd_0);
	}

	fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "6", 2); // GPIO Rover Power enable OBC pin
		close(fd_0);

		// Control direction: set to output
		int fd_1 = open("/sys/class/gpio/gpio6/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}
	}
	this->fd_actuator_rover = open("/sys/class/gpio/gpio6/value", O_WRONLY);
#endif
}

Actuator_Rover::~Actuator_Rover()
{
#ifndef LOCAL_DEV
	close(this->fd_actuator_rover);

	// Unexport GPIO pin
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "6", 2); // GPIO Rover Power enable OBC pin
		close(fd_0);
	}
#endif
}

int Actuator_Rover::getActutator_type()
{
	return ACTUATOR_LIST::ROVER_POWER;
}

void Actuator_Rover::enable()
{
#ifndef LOCAL_DEV
	int buffer = 49;

	// Write "1" to output
	write(this->fd_actuator_rover, "1", 1);
	
	close(this->fd_actuator_rover);
	this->fd_actuator_rover = open("/sys/class/gpio/gpio6/value", O_WRONLY);
#endif
}

void Actuator_Rover::disable()
{
#ifndef LOCAL_DEV
	int buffer = 48;

	// Write "0" to output
	write(this->fd_actuator_rover, "0", 1);

	close(this->fd_actuator_rover);
	this->fd_actuator_rover = open("/sys/class/gpio/gpio6/value", O_WRONLY);
#endif
}
