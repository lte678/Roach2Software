/* @file ReceiveHandler.cpp
 * @date 20.06.2019
 * @brief Interface for all classes to implement which want to be informed about received data from the communication links
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 **/

#include "RocketSignals.h"

RocketSignals *instance_rocket_signals; // Only use this here!! This is a solution to use the wiringPi interrupt function which cannot handle class instances

/*
 * @brief Constructor, accepts the required handler function to inform about received signals
 **/
RocketSignals::RocketSignals(ReceiveHandler *rhandler)
{
	this->handler = rhandler;
	instance_rocket_signals = this;
}

/*
 * @brief Tells which connection is used if the abstract baseclass Connection is used
 * @return int connection/link type index (UART, RS422/RXSM, Ethernet, GPIO interrupts)
**/
int RocketSignals::whichConnection() {
	return ROCKET_SIGNALS;
}

/**
 * @brief ISR for rocket signals, the "real" ISR is outside of this class due to limitation of wiringPi
*/
void RocketSignals::interrupt_routine(bool SOE, bool SODS, bool LO)
{
	if (SOE && this->enableSOE_ISR) {
		this->handler->rocketSignalReceived(0);
	}
	else if (SODS && this->enableSODS_ISR) {
		this->handler->rocketSignalReceived(1);
	}
	else if (LO && this->enableLO_ISR) {
		this->handler->rocketSignalReceived(2);
	}
}

/**
 * @brief Enable the recepetion of the LO signal through enabling the ISR handling
*/
void RocketSignals::enableLO_ISR()
{
	if (!this->SODS_ISR_enabled) {
		wiringPiISR(this->pin_LO, INT_EDGE_RISING, (*void)wiring_rocket_signals_interrupt_LO);
		this->SODS_ISR_enabled = true;
	}
}

/**
 * @brief Disable the recepetion of the LO signal through disabling the ISR handling
*/
void RocketSignals::disableLO_ISR()
{
	if (this->enableLO_ISR) {
		this->enableLO_ISR = false;
		// Could not find a deactive ISR function in wiringPi, so instead deactive the pass through of the ISR in this class
	}
}

/**
 * @brief Enable the recepetion of the SODS signal through enabling the ISR handling
*/
void RocketSignals::enableSODS_ISR()
{
	if (!this->enableSODS_ISR) {
		wiringPiISR(this->pin_SODS, INT_EDGE_RISING, (void*)wiring_rocket_signals_interrupt_SODS);
		this->enableSODS_ISR = true;
	}
}

/**
 * @brief Disable the recepetion of the LO signal through disabling the ISR handling
*/
void RocketSignals::disableSODS_ISR()
{
	if (this->enableSODS_ISR) {
		this->enableLO_ISR = false;
	}
}

/**
 * @brief Enable the recepetion of the SOE signal through enabling the ISR handling
*/
void RocketSignals::enableSOE_ISR()
{
	if (!this->enableSOE_ISR) {
		wiringPiISR(this->pin_SOE, INT_EDGE_RISING, (*void)wiring_rocket_signals_interrupt_SOE);
		this->enableSOE_ISR = true;
	}
}

/**
 * @brief Disable the recepetion of the SOE signal through disabling the ISR handling
*/
void RocketSignals::disableSOE_ISR()
{
	if (this->enableSOE_ISR) {
		this->enableSOE_ISR = false;
	}
}

// Actual ISR for wiringPi, not part of the class
// This is the reason that one static instance of RocketSignals needs to hold in this file

/*
* @brief Interrupt for LO signal
*/
void wiring_rocket_signals_interrupt_LO(void) {
	instance_rocket_signals->interrupt_routine(false, false, true);
}

/*
* @brief Interrupt for SOE signal
*/
void wiring_rocket_signals_interrupt_SOE(void) {
	instance_rocket_signals->interrupt_routine(true, false, false);
}

/*
* @brief Interrupt for SODS signal
*/
void wiring_rocket_signals_interrupt_SODS(void) {
	instance_rocket_signals->interrupt_routine(false, true, false);
}
