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

#include <queue>

#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"
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

class UART :
	public Connection
{
private:
	// Thread related
	std::atomic<bool> stop_bit;
	std::mutex lock_send_queue;
	std::mutex lock_receive_queue;
	std::queue<Data_super*> send_queue;
	std::queue<Data_simple*> receive_queue;

	const int max_number_packages_send_at_once = 1000; // Max number of packages to send at once
	const int rx_loop_wait = 1; // Time to wait after each check of RX buffer
#ifndef LOCAL_DEV
	const char* port = "/dev/ttyS1";
#else
	const char* port = "/dev/ttyS0";
#endif
	struct termios tty;
	int baud;
	int serial_port;
	int numberPackagesReceived;
	int numberPackagesReceivedInvalid;
	int numberPackagesSend;
	uint64_t* tx_frames;
	uint64_t* rx_frames;
	int frame_counter_tx;
	int frame_counter_rx;
	Data_super* dataToSend;
	Data_super* dataReceived;
	char* rx_buffer;
	uint rx_buffer_counter;
	int numberDataToSend;
	int numberDataReceived;
	bool send_ongoing;
	bool receive_ongoing;
	void send(void);
	uint16_t calc_crc(const uint8_t* data, uint16_t size);
public:
	UART();
	~UART();
	void run();
	void sendData(Data_super** data, int count);
	Data_simple* getData(void);
	int getNumberReceived();
	int whichConnection();
	void receive(void);
	void stop_thread();
	bool isData_received();
};

#endif
