/* @file Connection.h
 * @date 06.07.19
 * @brief Interface for all communications to identify them
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */
#ifndef ROACH2_COMM_CONNECTION_HEADER_FILE
#define ROACH2_COMM_CONNECTION_HEADER_FILE


enum CONNECTION_TYPES {
	RS422 = 0,
	ROCKET_SIGNALS = 1,
	UART_LINK = 2,
	ETHERNET = 3
};


class Connection
{
public:
	/*
	 * @brief Returns an index to identify which connection link is used, see also the enum CONNECTION_TYPES in this file
	 * @return int connection link type (RS422, rocket signals, UART, Ethernet)
	*/
	virtual int whichConnection() = 0;
};

#endif 
