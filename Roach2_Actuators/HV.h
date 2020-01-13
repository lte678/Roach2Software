#ifndef _Actuator_HV_h_
#define _Actuator_HV_h_

#include "Actuator.h"
#include "../Roach2_Hardware/GPIODevice.h"

#include <iostream>

class Actuator_HV : public Actuator
{
private:
	GPIODevice* hvLeft;
	GPIODevice* hvRight;
public:
	Actuator_HV();
	~Actuator_HV();
	int getActuatorType() override;
	void enable(bool debug) override;
	void disable(bool debug) override;
};

#endif 
