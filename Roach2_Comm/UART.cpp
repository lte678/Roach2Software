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
	this->send();
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
