#include "Actuator.h"

Actuator::Actuator()
{
	isEnabled = false;
	isDebugMode = false;
}

Actuator::~Actuator()
{

}

void Actuator::enableDebugMode()
{
	this->isDebugMode = true;
}

void Actuator::disableDebugMode()
{
	this->isDebugMode = false;
}
