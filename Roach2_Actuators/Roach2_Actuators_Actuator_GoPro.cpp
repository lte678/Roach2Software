#include "Roach2_Actuators_Actuartor_GoPro.h"

Actuator_GoPro::Actuator_GoPro() : Actuator()
{
	this->gopro_enabled = false;

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

		// Control direction: set to input
		int fd_1 = open("/sys/class/gpio/gpio0/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "in", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio1/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "in", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to output
		fd_1 = open("/sys/class/gpio/gpio200/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to output
		fd_1 = open("/sys/class/gpio/gpio2/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "out", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		// Control direction: set to output
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

		// GoPro signals are low-active, so set to high
		this->disable();
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
		int res0 = write(fd_0, "200", 2); // GPIO CAM supply OBC pin
		usleep(1000);
		res0 = write(fd_0, "0", 2); // GPIO GoPro1 OBC pin
		res0 = write(fd_0, "1", 2); // GPIO GoPro2 OBC pin
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
	write(this->fd_camsupply, "1", 1);
	usleep(10000);

	// Control direction: set to input
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

	this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_WRONLY);
	this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_WRONLY);

	// Write "1" to output
	write(this->fd_gopro1, "0", 1);
	write(this->fd_gopro2, "0", 1);
	write(this->fd_lights1, "1", 1);
	write(this->fd_lights2, "1", 1);

	// Reopen device to reset write pointer
	close(this->fd_camsupply);
	close(this->fd_lights1);
	close(this->fd_lights2);
	this->fd_camsupply = open("/sys/class/gpio/gpio200/value", O_WRONLY);
	this->fd_lights1 = open("/sys/class/gpio/gpio2/value", O_WRONLY);
	this->fd_lights2 = open("/sys/class/gpio/gpio3/value", O_WRONLY);

	// Note: GoPro signals are only trigger signals
	usleep(500 * 1000); // wait 700ms

	// Set GoPro pins to floating
	// Control direction: set to input
	fd_1 = open("/sys/class/gpio/gpio0/direction", O_WRONLY);
	if (fd_1 != -1) {
		int res = write(fd_1, "in", 2);
		close(fd_1);
	}
	else {
		// Error!
	}
	// Control direction: set to input
	fd_1 = open("/sys/class/gpio/gpio1/direction", O_WRONLY);
	if (fd_1 != -1) {
		int res = write(fd_1, "in", 2);
		close(fd_1);
	}
	else {
		// Error!
	}
	close(this->fd_gopro1);
	close(this->fd_gopro2);
	this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_RDONLY);
	this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_RDONLY);

	this->gopro_enabled = true;
}

void Actuator_GoPro::disableGoPro() {

	if (this->gopro_enabled) {

		// Control direction: set to input
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

		this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_WRONLY);
		this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_WRONLY);

		// Write "0" to output
		write(this->fd_gopro1, "0", 1);
		write(this->fd_gopro2, "0", 1);
		close(this->fd_gopro1);
		close(this->fd_gopro2);

		// Note: GoPro signals are only trigger signals
		usleep(2300 * 1000); // wait 2500ms

		// Set GoPro pins to floating
		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio0/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "in", 3);
			close(fd_1);
		}
		else {
			// Error!
		}
		// Control direction: set to input
		fd_1 = open("/sys/class/gpio/gpio1/direction", O_WRONLY);
		if (fd_1 != -1) {
			int res = write(fd_1, "in", 3);
			close(fd_1);
		}
		else {
			// Error!
		}

		this->fd_gopro1 = open("/sys/class/gpio/gpio0/value", O_RDONLY);
		this->fd_gopro2 = open("/sys/class/gpio/gpio1/value", O_RDONLY);

		this->gopro_enabled = false;
	}
}

void Actuator_GoPro::disable()
{
	write(this->fd_camsupply, "1", 1);
	write(this->fd_lights1, "0", 1);
	write(this->fd_lights2, "0", 1);

	// Reopen device to reset write pointer
	close(this->fd_camsupply);
	close(this->fd_lights1);
	close(this->fd_lights2);
	close(this->fd_gopro1);
	close(this->fd_gopro2);
	this->fd_camsupply = open("/sys/class/gpio/gpio200/value", O_WRONLY);
	this->fd_lights1 = open("/sys/class/gpio/gpio2/value", O_WRONLY);
	this->fd_lights2 = open("/sys/class/gpio/gpio3/value", O_WRONLY);
}
