#include "Actuator_Engine.h"

Actuator_Engine::Actuator_Engine() : Actuator()
{
	PWM_freq = 0.0;
}

Actuator_Engine::~Actuator_Engine()
{

}

double Actuator_Engine::getPWM_freq()
{
	return(this->PWM_freq);
}

void Actuator_Engine::setPWM_freq(double PWM_freq)
{
	this->PWM_freq = PWM_freq;
}