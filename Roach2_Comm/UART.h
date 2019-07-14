/* @file UART.h
 * @date 14.07.2019
 * @brief UART communication link class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#ifndef UART_HEADER_FILE
#define UART_HEADER_FILE

#include <wiringSerial.h>
#include "Connection.h"
#include "../Roach2_DataStore/data.h"

class UART :
	public Connection
{
private:
	char* port;
	int baud;
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
};

#endif