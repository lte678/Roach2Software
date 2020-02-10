/* @file UART.h
 * @date 14.07.2019
 * @brief UART communication link class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#ifndef UART_HEADER_FILE
#define UART_HEADER_FILE


 // Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <unistd.h>
#include <memory>
#include <cstring>

#include <queue>

#include "../Roach2_DataStore/Data_super.h"
#include "../Roach2_DataStore/Data_Raw.h"
#include "Connection.h"

// Threading related
#include <thread>
#include <deque>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <math.h>

#define CRC16 0x8005 // CRC16 polynom

class UART : public Connection {
private:
#ifndef LOCAL_DEV
    const char* port = "/dev/ttyS1";
#else
    const char* port = "/dev/ttyS0";
#endif

	// Thread related
	std::atomic<bool> stop_bit;
	std::mutex lock_send_queue;
	std::mutex lock_receive_queue;
	std::queue<std::unique_ptr<Data_super>> send_queue;
	std::queue<std::unique_ptr<Data_super>> receive_queue;

	struct termios tty;
	int serial_port;
	int numberPackagesReceived;
	int frame_counter_rx;
	int crc_failure_counter_rx;
	char* rx_buffer;
	uint rx_buffer_counter;
	int numberDataReceived;
	std::mutex send_lock;
	void send();
	PLATFORM origin;
	static const uint8_t syncword;

	uint16_t calc_crc(const uint8_t* data, uint16_t size);
public:
	explicit UART(PLATFORM _origin);
	~UART();
	void run();
	void sendData(std::unique_ptr<Data_super> data);
	std::unique_ptr<Data_super> getData();
	int getNumberReceived();
	int whichConnection() override;
	void receive();
	void stop_thread();
	bool isData_received();
};

#endif
