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
	ROVER_POWER = 2,
	STATUS_LEDS_OBC = 3,
	STATUS_LEDS_RCU = 4,
	ENGINE_PWM = 5
};

class Actuator
{
public:
	Actuator();
	~Actuator();
	virtual void enable(bool debug) = 0;
	virtual void disable(bool debug) = 0;
	void enableDebugMode();
	void disableDebugMode();
	virtual int getActuatorType() = 0;
protected:
	bool isEnabled;
    bool isDebugMode;
};

#endif


