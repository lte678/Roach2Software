/* @file UART.cpp
 * @date 14.07.2019
 * @brief UART communication link class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#include "UART.h"

UART::UART()
{
	// Try to open serial port
	// Normally: /dev/ttyUSB0 or similar
	serial_port = open(this->port, O_RDWR);

	// Check if open was done
	// Check also that use is in dialout group: sudo adduser $USER dialout
	if (serial_port < 0) {
		// Error occured
		printf("Error %i from open: %s\n", errno, strerror(errno));
	}
	else {
		// Configure serial port
		memset(&tty, 0, sizeof(tty));
		if (tcgetattr(serial_port, &tty) == 0) {
			// No error, config loaded
			tty.c_cflag &= ~PARENB; // No parity: clear parity bit
			tty.c_cflag &= ~CSTOPB; // One stop bit
			tty.c_cflag |= CS8; // 8bits per byte
			tty.c_cflag |= CRTSCTS; // No hardware flow control
			tty.c_cflag |= CREAD | CLOCAL; // disable modem specific signals and acitve data reading (CREAD)
			tty.c_lflag &= ~ICANON; // Disable canonical mode to prevent to miss some bytes with special meaning (possibly)
			tty.c_lflag &= ~ECHONL; // Disable echo functionality
			tty.c_lflag &= ~ISIG; // Don't interpret any special characters
			tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
			tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special handling of received bytes
			tty.c_oflag &= ~OPOST; // Disable output signal handling
			tty.c_oflag &= ~ONLCR; // Disable output conversion of newline signal
			tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
			tty.c_cc[VMIN] = 0;
			cfsetispeed(&tty, B115200); // Baudrate input
			cfsetospeed(&tty, B115200); // Baudrate output
			// Save tty settings, also checking for error
			if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
				// Error occured
			}
			char* msg = "TEST MESSAGE 1";
			while (true) {
				write(serial_port, msg, sizeof(msg));
				sleep(1);
			}
		}
		else {
			// Configuration load failed
		}
	}
}


UART::~UART()
{
}

/**
 * @brief Puts the given data to the list of data to send and transmits the data afterwards. If some data is already in the queue, this data will be lost!
 * @param data pointer to Data objects to send
 * @param int number of Data objects passed to this function
*/
void UART::sendData(Data * data, int count)
{
	this->dataToSend = data;
	numberDataToSend = count;
}

/**
 * @brief Returns the received data. If new data is received afterwards, the system will return the new data.
 * @return pointer to the received data
*/
Data * UART::getData(void)
{
	this->numberDataReceived = 0;
	return this->dataReceived;
}

/**
* @brief Returns the number of received packages converted into Data objects, which can be access with getData() function
* @return int number of received Data objects available for access
*/
int UART::getNumberReceived()
{
	return this->numberDataReceived;
}

int UART::whichConnection()
{
	return CONNECTION_TYPES::UART_LINK;
}
