#ifndef _Actuator_h_
#define _Actuator_h_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum ACTUATOR_LIST {
	GOPRO = 0,
	HV_GEN = 1,
	ROVER_PG = 2,
	STATUS_LEDS_OBC = 3,
	STATUS_LEDS_RCU = 4
};

class Actuator
{
public:
	Actuator();
	~Actuator();
	virtual void enable() = 0;
	virtual void disable() = 0;
	void enableDebugMode();
	void disableDebugMode();
	virtual int getActutator_type() = 0;
private:
	bool isDebugMode;
protected:
	bool isEnabled;
	
	
};

#endif


