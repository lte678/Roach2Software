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

		// Check whether Data_simple is used instead of Data
		Data_super* dataToSend = this->send_queue.front();
		std::vector<uint64_t> tx_data = dataToSend->convert_to_serial();
		int data_length = dataToSend->convert_to_serial_array_length();
		this->tx_frames = new uint64_t[data_length]; // each frame consists of 11 bytes
		for (int i = 0; i < data_length; i++) {
			this->tx_frames[i] = tx_data[i];
		}
			 
		// write to UART
		uint64_t buffer_data;
		u_int16_t crc;
		uint8_t header_data;
		for (int i = 0; i < data_length; i++) {
			buffer_data = this->tx_frames[i];

			// Create current header section
			header_data = data_length;
			header_data += ((uint8_t)this->frame_counter_tx << 4); // Shift to upper 4bits

			// Calc CRC
			uint8_t* crc_data = (uint8_t*)& buffer_data;
			crc = this->calc_crc(crc_data, 8);

			// UART transmission are LSB first, this must be handled by the receiver
			write(this->serial_port, &header_data, sizeof(header_data));
			write(this->serial_port, &buffer_data, sizeof(buffer_data));
			write(this->serial_port, &crc, sizeof(crc));
			this->frame_counter_tx++;
		}
		this->send_ongoing = false;

		// Free memory
		this->send_queue.pop();
		delete dataToSend;
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
 * This function cannot run in parallel to the main thread and is therefore replaced by run(data_ref).
*/
void UART::receive(void)
{
	uint16_t crc;
	uint frame_nr;
	uint frame_length;
	uint64_t data;


	uint8_t read_buf[11]; // one frame is 88bits long
	memset(&read_buf, '\0', sizeof(read_buf));

	// Read, this call maybe blocking the thread, depending configuration (VMIN and VTIME)
	int n = read(this->serial_port, &read_buf, sizeof(read_buf));

	// Copy to internal buffer if no complete frame received
	if (n > 0) {
		for (int i = 0; i < n; i++) {
			this->rx_buffer[this->rx_buffer_counter + i] = read_buf[i];
		}
		this->rx_buffer_counter += n;
	}

	// Frame parser
	if (this->rx_buffer_counter >= 11) {
		// Parse into command structure
		if (this->frame_counter_rx == 0) {
			// New data received
			// Read structure
			frame_nr = (this->rx_buffer[0] & 0xF0);
			frame_length = (this->rx_buffer[0] & 0x0F);
			
			for (int i = 0; i < 8; i++) {
				data = (data << 8) | this->rx_buffer[1 + i];
			}
			crc = (this->rx_buffer[9] << 8);
			crc = crc + this->rx_buffer[10];

			this->numberDataReceived++;
			this->numberPackagesReceived++;
			
			this->rx_buffer_counter = 0;

			// Split incoming message
			uint32_t cmd = ((data & 0xFFFFFFFF00000000) >> 32);
			uint32_t para = (data & 0x00000000FFFFFFFF);

			Data_simple* data_obj = new Data_simple(cmd, para);
			this->receive_queue.push(data_obj);
		}
		else {
			// Some parts missing
		}
	}
}

/**
 * @brief Stops the running communication thread
*/
void UART::stop_thread()
{
	this->stop_bit.store(true);
}

/**
 * @brief Checks if data was received
 * @return true if data was received, otherwise false (also in case the receive queue is used by the communication thread)
*/
bool UART::isData_received()
{
	// Try to get access
	bool result = this->lock_receive_queue.try_lock();
	if (result) {
		// got lock
		result = !this->receive_queue.empty();

		// release lock
		this->lock_receive_queue.unlock();
	}
	return result;
}

UART::UART()
{
    std::cout << "[UART] Initializing parameters" << std::endl;

	// Init
	this->numberDataReceived = 0;
	this->numberPackagesReceived = 0;
	this->numberPackagesReceivedInvalid = 0;
	this->numberPackagesSend = 0;
	this->send_ongoing = false;
	this->receive_ongoing = false;

	this->initEventHandlerArray(); // Required to use event handlers

	// Try to open serial port
	// Normally: /dev/ttyUSB0 or similar
	serial_port = open(this->port, O_RDWR);
    std::cout << "[UART] Port acquired!" << std::endl;

	// Check if open was done
	// Check also that use is in dialout group: sudo adduser $USER dialout
	if (serial_port < 0) {
		// Error occured
        std::cout << "[UART] Error opening serial port!" << std::endl;
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
			tty.c_cc[VTIME] = 1;    // Wait for up to 0.1s (1 deciseconds), returning as soon as any data is received.
			tty.c_cc[VMIN] = 0;
			cfsetispeed(&tty, B38400); // Baudrate input
			cfsetospeed(&tty, B38400); // Baudrate output

			// RX internal buffer
			this->rx_buffer = new char[11];
			this->rx_buffer_counter = 0;

			// Save tty settings, also checking for error
			if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
				// Error occured
			}
		}
		else {
			// Configuration load failed
            std::cout << "[UART] Failed to set configuration" << std::endl;
		}
	}
}


UART::~UART()
{
}

void UART::run()
{
	// Init

	// Main loop
	while (!this->stop_bit.load()) {
		// Check if data was received
		this->lock_receive_queue.lock();
		this->receive();
		this->lock_receive_queue.unlock();

		// Check if data to send
		// Acquire lock
		this->lock_send_queue.lock();
		
		while (!this->send_queue.empty()) {
			this->send();
		}

		// Release lock
		this->lock_send_queue.unlock();

		// Wait for 100us => thus we would reach around 10Kpackages/s, even more depending on the UART buffer size
		usleep(100);
	}
}

/**
 * @brief Puts the given data to the list of data to send and transmits the data afterwards. If some data is already in the queue, this data will be lost!
 *		  Note: Access is blocking on send queue!
 * @param data pointer to Data objects to send
 * @param int number of Data objects passed to this function
*/
void UART::sendData(Data_super** data, int count)
{
	// Acquire lock
	this->lock_send_queue.lock();

	// Add pointer to send queue
	for (int i = 0; i < count; i++) {
		this->send_queue.push(data[i]);
	}

	this->lock_send_queue.unlock();
}

/**
 * @brief Returns the received data. If new data is received afterwards, the system will return the new data.
 *        Non-Blocking access!
 * @return pointer to the received data or nullptr if no lock could be acquired or no data is available
*/
Data_simple* UART::getData()
{
	bool res = this->lock_receive_queue.try_lock();

	// Try to load from queue if lock could be acquired
	if (res) {
		Data_simple* copy;
		if (!this->receive_queue.empty()) {
			copy = this->receive_queue.front();
			this->receive_queue.pop();
		}
		else {
			// No data available
			copy = nullptr;
		}
		// Release lock
		this->lock_receive_queue.unlock();
		return copy;
	}
	else {
		// No lock acquired
		return nullptr;
	}
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
