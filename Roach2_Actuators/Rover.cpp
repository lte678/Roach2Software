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
    if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|Rover] Rover power enable" << std::endl;
        roverPowerPin->write(true); // Active low
    }
}

void Actuator_Rover::disable(bool debug)
{
    if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|Rover] Rover power disable" << std::endl;
        roverPowerPin->write(false); // Active low
    }
}
