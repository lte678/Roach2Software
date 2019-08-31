/* @file Connection.h
 * @date 06.07.19
 * @brief Interface for all communications to identify them
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */
#ifndef ROACH2_COMM_CONNECTION_HEADER_FILE
#define ROACH2_COMM_CONNECTION_HEADER_FILE

#include "ReceiveHandler.h"

enum CONNECTION_TYPES {
	RS422 = 0,
	ROCKET_SIGNALS = 1,
	UART_LINK = 2,
	ETHERNET = 3
};

class Connection
{
private:
	ReceiveHandler** event_handler;
	int number_event_handler = 0;
public:
	/*
	 * @brief Returns an index to identify which connection link is used, see also the enum CONNECTION_TYPES in this file
	 * @return int connection link type (RS422, rocket signals, UART, Ethernet)
	*/
	virtual int whichConnection() = 0;

	void initEventHandlerArray() {
		this->event_handler = new ReceiveHandler*[10];
		this->number_event_handler = 0;
	}

	void addEventHandler(ReceiveHandler* handler) {
		this->event_handler[this->number_event_handler] = handler;
		this->number_event_handler++;
	}

	ReceiveHandler** getRegisteredEventHandler(int &counter_buffer) {
		counter_buffer = this->number_event_handler;
		return this->event_handler;
	}
};

#endif 
