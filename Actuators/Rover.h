#ifndef ACTUATOR_ROVER_H
#define ACTUATOR_ROVER_H

#include <iostream>

#include "Actuator.h"
#include "../Hardware/GPIODevice.h"

class Actuator_Rover :
	public Actuator
{
private:
    GPIODevice* roverPowerPin;
public:
	Actuator_Rover();
	~Actuator_Rover();
	int getActuatorType() override;
	void enable(bool debug) override;
	void disable(bool debug) override;
protected:

};

#endif