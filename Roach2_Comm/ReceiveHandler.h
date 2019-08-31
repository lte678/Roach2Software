/* @file ReceiveHandler.h
 * @date 20.06.2019
 * @brief Interface for all classes to implement which want to be informed about received data from the communication links
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#ifndef RECEIVE_HANDLER_HEADER
#define RECEIVE_HANDLER_HEADER

// Definition of REXUS signals
enum class REXUS_SIGNALS {
	LO = 0,
	SODS = 1,
	SOE = 2
};

class ReceiveHandler
{
public:
	/*
	 * @brief Function called when data was received through an Ethernet link
	**/
	virtual void packageReceivedEthernet() = 0;
	/* 
	 * @brief Function called when data was received from the Rexus service module link
	 **/
	virtual void packageReceivedRexus(uint64_t message, int msg_length) = 0;
	/*
	 * @brief Function called when a rocket signal was detected
	**/
	virtual void rocketSignalReceived(int signal_source) = 0;
	/*
	 * @brief Function called when data from the UART port was received 
	 **/
	virtual void packageReceivedUART(uint64_t message, int msg_length) = 0;
};

#endif

