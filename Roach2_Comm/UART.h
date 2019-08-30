/* @file UART.h
 * @date 14.07.2019
 * @brief UART communication link class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#ifndef UART_HEADER_FILE
#define UART_HEADER_FILE

#include <stdio.h>
#include <string.h>
 // Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <unistd.h>
#include "..//Roach2_DataStore/data.h"

#include "Connection.h"

#define CRC16 0x8005 // CRC16 polynom

class UART :
	public Connection
{
private:
	char* port = "/dev/ttyS2";
	struct termios tty;
	int baud;
	int serial_port;
	int numberPackagesReceived;
	int numberPackagesReceivedInvalid;
	int numberPackagesSend;
	long* tx_frames;
	long* rx_frames;
	int frame_counter_tx;
	int frame_counter_rx;
	Data* dataToSend;
	Data* dataReceived;
	int numberDataToSend;
	int numberDataReceived;
	bool send_ongoing;
	bool receive_ongoing;
	void send(void);
	void receive(void);
	uint16_t calc_crc(const uint8_t* data, uint16_t size);
public:
	UART();
	~UART();
	void sendData(Data *data, int count);
	Data* getData(void);
	int getNumberReceived();
	int whichConnection();
};

#endif
