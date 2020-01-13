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
        hvLeft->write(true);
        hvRight->write(true);
    }
}

void Actuator_HV::disable(bool debug)
{
    if(!isDebugMode || debug) {
        hvLeft->write(false);
        hvRight->write(false);
    }
}
