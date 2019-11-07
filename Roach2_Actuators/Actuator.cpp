#include "Actuator.h"

Actuator::Actuator()
{
	isEnabled = false;
	isDebugMode = false;
	state = 0;
}

Actuator::~Actuator() 
{

}

int Actuator::getState()
{
	return(this->state);
}

void Actuator::setState(int state)
{
	this->state = state;
}

void Actuator::enable()
{
	this->isEnabled = true;
}

void Actuator::disable()
{
	this->isEnabled = false;
}

void Actuator::enableDebugMode()
{
	this->isDebugMode = true;
}

void Actuator::disableDebugMode()
{
	this->isDebugMode = false;
}

void Actuator::trigger()
{

}
