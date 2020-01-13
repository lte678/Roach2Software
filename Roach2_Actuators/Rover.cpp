#include "Rover.h"

Actuator_Rover::Actuator_Rover()
{
    std::cout << "[Actuator|Rover] Initializing" << std::endl;
    roverPowerPin = new GPIODevice(6, GPIODevice::OUTPUT);
}

Actuator_Rover::~Actuator_Rover()
{
    delete roverPowerPin;
}

int Actuator_Rover::getActuatorType()
{
	return ACTUATOR_LIST::ROVER_POWER;
}

void Actuator_Rover::enable(bool debug)
{
    if(!isDebugMode || debug) {
        roverPowerPin->write(true);
    }
}

void Actuator_Rover::disable(bool debug)
{
    if(!isDebugMode || debug) {
        roverPowerPin->write(false);
    }
}
