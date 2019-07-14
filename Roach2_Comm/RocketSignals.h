#ifndef ROCKET_SIGNALS_HEADER_FILE
#define ROCKET_SIGNALS_HEADER_FILE

#include "Connection.h"
#include "ReceiveHandler.h"
#include <wiringPi>

class RocketSignals :
	public Connection
{
private:
	bool LO_ISR_enabled;
	bool SODS_ISR_enabled;
	bool SOE_ISR_enabled;
	int const pin_LO = 0;
	int const pin_SODS = 1;
	int const pin_SOE = 2;
	ReceiveHandler *handler;
public:
	RocketSignals(ReceiveHandler *handler);
	int whichConnection(void);
	void interrupt_routine(bool SOE, bool SODS, bool LO);
	void enableLO_ISR();
	void disableLO_ISR();
	void enableSODS_ISR();
	void disableSODS_ISR();
	void enableSOE_ISR();
	void disableSOE_ISR();
};

// Interrupt function for wiringPi
void wiring_rocket_signals_interrupt_LO(void);
void wiring_rocket_signals_interrupt_SOE(void);
void wiring_rocket_signals_interrupt_SODS(void);

#endif

