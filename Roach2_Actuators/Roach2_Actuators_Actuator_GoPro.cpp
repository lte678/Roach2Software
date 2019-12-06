#include "Roach2_Actuators_Actuartor_GoPro.h"

Actuator_GoPro::Actuator_GoPro() : Actuator()
{
	// Unexport GPIO pins
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "0", 2); // GPIO GoPro1 OBC pin
		res0 = write(fd_0, "1", 2); // GPIO GoPro2 OBC pin
		res0 = write(fd_0, "200", 2); // GPIO CAM supply OBC pin
		res0 = write(fd_0, "2", 2); // GPIO lights 1 OBC pin
		res0 = write(fd_0, "3", 2); // GPIO lights 2 OBC pin
		close(fd_0);
	}

	// Export GPIO pins
	fd_0 = open("/sys/class/gpio/export", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "0", 2); // GPIO GoPro1 OBC pin
		res0 = write(fd_0, "1", 1); // GPIO GoPro2 OBC pin
		res0 = write(fd_0, "200", 3); // GPIO CAM supply OBC pin
		res0 = write(fd_0, "2", 1); // GPIO lights 1 OBC pin
		res0 = write(fd_0, "3", 1); // GPIO lights 2 OBC pin
		close(fd_0);

		// Control direction: set to output
		int fd_1 = open("/sys/class/gpio/gpio0/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio1/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio200/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio2/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio3/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Open the devices
		this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_WRONLY);
		this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_WRONLY);
		this->fd_camsupply = open("/sys/class/gpio/gpio200/value", O_WRONLY);
		this->fd_lights1 = open("/sys/class/gpio/gpio2/value", O_WRONLY);
		this->fd_lights2 = open("/sys/class/gpio/gpio3/value", O_WRONLY);
	}
}

Actuator_GoPro::~Actuator_GoPro()
{
	close(this->fd_gopro1);
	close(this->fd_gopro2);
	close(this->fd_camsupply);
	close(this->fd_lights1);
	close(this->fd_lights2);

	// Unexport GPIO pins
	int fd_0 = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd_0 != -1) {
		int res0 = write(fd_0, "0", 2); // GPIO GoPro1 OBC pin
		res0 = write(fd_0, "1", 2); // GPIO GoPro2 OBC pin
		res0 = write(fd_0, "200", 2); // GPIO CAM supply OBC pin
		res0 = write(fd_0, "2", 2); // GPIO lights 1 OBC pin
		res0 = write(fd_0, "3", 2); // GPIO lights 2 OBC pin
		close(fd_0);
	}
}

int Actuator_GoPro::getActutator_type()
{
	return ACTUATOR_LIST::GOPRO;
}

void Actuator_GoPro::enable()
{
	int buffer = 49;

	// Write "1" to output
	write(this->fd_gopro1, "1", 1);
	write(this->fd_gopro2, "1", 1);
	write(this->fd_camsupply, "1", 1);
	write(this->fd_lights1, "1", 1);
	write(this->fd_lights2, "1", 1);

	// Reopen device to reset write pointer
	close(this->fd_gopro1);
	close(this->fd_gopro2);
	close(this->fd_camsupply);
	close(this->fd_lights1);
	close(this->fd_lights2);
	this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_WRONLY);
	this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_WRONLY);
	this->fd_camsupply = open("/sys/class/gpio/gpio200/value", O_WRONLY);
	this->fd_lights1 = open("/sys/class/gpio/gpio2/value", O_WRONLY);
	this->fd_lights2 = open("/sys/class/gpio/gpio3/value", O_WRONLY);
}

void Actuator_GoPro::disable()
{
	int buffer = 48;

	// Write "1" to output
	write(this->fd_gopro1, &buffer, 1);
	write(this->fd_gopro2, &buffer, 1);
	write(this->fd_camsupply, &buffer, 1);
	write(this->fd_lights1, &buffer, 1);
	write(this->fd_lights2, &buffer, 1);

	// Reopen device to reset write pointer
	close(this->fd_gopro1);
	close(this->fd_gopro2);
	close(this->fd_camsupply);
	close(this->fd_lights1);
	close(this->fd_lights2);
	this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_WRONLY);
	this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_WRONLY);
	this->fd_camsupply = open("/sys/class/gpio/gpio200/value", O_WRONLY);
	this->fd_lights1 = open("/sys/class/gpio/gpio2/value", O_WRONLY);
	this->fd_lights2 = open("/sys/class/gpio/gpio3/value", O_WRONLY);
}
