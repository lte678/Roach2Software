/* @file UART.cpp
 * @date 14.07.2019
 * @brief UART communication link class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#include "UART.h"

/**
 * @brief Send data according to our protocol
*/
void UART::send(void)
{
	if (this->send_ongoing) {
		// Wait until previous send operation done
	}
	else {
		// Send next data
		this->frame_counter_tx = 0;

		this->send_ongoing = true;
		// Convert into frame structure:
		// 64bit data, 16bit CRC, 4bit frame number, 4bit data length
		long* tx_data = this->dataToSend->convert_to_serial();
		int data_length = this->dataToSend->convert_to_serial_array_length();
		this->tx_frames = new long[data_length]; // each frame consists of 11 bytes
		for (int i = 0; i < data_length; i++) {
			this->tx_frames[i] = tx_data[i];
		}
			 
		// write to UART
		long buffer_data;
		u_int16_t crc;
		uint8_t header_data;
		for (int i = 0; i < data_length; i++) {
			buffer_data = this->tx_frames[i];

			// Create current header section
			header_data = data_length;
			header_data += ((uint8_t)this->frame_counter_rx << 4); // Shift to upper 4bits

			// Calc CRC
			uint8_t* crc_data = (uint8_t*)& buffer_data;
			crc = this->calc_crc(crc_data, 8);

			write(this->serial_port, &header_data, sizeof(header_data));
			write(this->serial_port, &buffer_data, sizeof(buffer_data));
			write(this->serial_port, &crc, sizeof(crc));
			this->frame_counter_tx++;
		}
	}
}

/**
 * @brief Function to calculate CRC16, see https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
 * @param data array
 * @param length of data array
 * @return checksum
*/
uint16_t UART::calc_crc(const uint8_t * data, uint16_t size)
{
	uint16_t out = 0;
	int bits_read = 0, bit_flag;

	/* Sanity check: */
	if (data == NULL)
		return 0;

	while (size > 0)
	{
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

		/* Increment bit counter: */
		bits_read++;
		if (bits_read > 7)
		{
			bits_read = 0;
			data++;
			size--;
		}

		/* Cycle check: */
		if (bit_flag)
			out ^= CRC16;

	}

	// item b) "push out" the last 16 bits
	int i;
	for (i = 0; i < 16; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if (bit_flag)
			out ^= CRC16;
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	i = 0x8000;
	int j = 0x0001;
	for (; i != 0; i >>= 1, j <<= 1) {
		if (i & out) crc |= j;
	}

	return crc;
}

/**
 * @brief Receive data from serial port according to our protocol
*/
void UART::receive(void)
{
}

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
