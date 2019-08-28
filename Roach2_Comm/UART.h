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
	Data* dataToSend;
	Data* dataReceived;
	int numberDataToSend;
	int numberDataReceived;
	void send(void);
	void receive(void);
public:
	UART();
	~UART();
	void sendData(Data *data, int count);
	Data* getData(void);
	int getNumberReceived();
	int whichConnection();
};

#endif
