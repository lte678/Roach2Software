#include "HV.h"

Actuator_HV::Actuator_HV() : Actuator()
{
    std::cout << "[Actuator|HV] Initializing" << std::endl;

    hvLeft = new GPIODevice(201, GPIODevice::OUTPUT);
    hvRight = new GPIODevice( 203, GPIODevice::OUTPUT);
}

Actuator_HV::~Actuator_HV()
{
    delete hvLeft;
    delete hvRight;
}

int Actuator_HV::getActuatorType()
{
	return ACTUATOR_LIST::HV_GEN;
}

void Actuator_HV::enable(bool debug)
{
    if(!isDebugMode || debug) {
        std::cout << "[Actuator|HV] Enabling" << std::endl;
        hvLeft->write(true);
        hvRight->write(true);
    }
}

void Actuator_HV::disable(bool debug)
{
    if(!isDebugMode || debug) {
        std::cout << "[Actuator|HV] Disabling" << std::endl;
        hvLeft->write(false);
        hvRight->write(false);
    }
}
