#pragma once
#ifndef _Actuator_Engine_h_
#define _Actuator_Engine_h_

#include "Actuator.h"

class Actuator_Engine : public Actuator
{
public:
	Actuator_Engine();
	~Actuator_Engine();
	double getPWM_freq();
	void setPWM_freq(double PWM_freq);

private:
	double PWM_freq;

protected:

};

#endif 
