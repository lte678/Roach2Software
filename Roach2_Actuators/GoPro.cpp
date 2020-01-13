#include "GoPro.h"

Actuator_GoPro::Actuator_GoPro() : Actuator()
{
    std::cout << "[Actuator|GoPro] Initializing" << std::endl;
	gopro_enabled = false;

    gopro1 = new GPIODevice(0, GPIODevice::INPUT);
    gopro2 = new GPIODevice(1, GPIODevice::INPUT);
    camsupply = new GPIODevice(200, GPIODevice::OUTPUT);
    lights1 = new GPIODevice(2, GPIODevice::OUTPUT);
    lights2 = new GPIODevice(3, GPIODevice::OUTPUT);
}

Actuator_GoPro::~Actuator_GoPro()
{
    delete gopro1;
    delete gopro2;
    delete camsupply;
    delete lights1;
    delete lights2;
}

int Actuator_GoPro::getActuatorType()
{
	return ACTUATOR_LIST::GOPRO;
}

void Actuator_GoPro::enable(bool debug)
{
    if(isDebugMode && !debug) {
        return;
    }

    camsupply->write(true);
    lights1->write(true);
    lights2->write(true);
	usleep(100 * 1000); // Wait 100ms for power on

	// Write "1" to output
	gopro1->setMode(GPIODevice::OUTPUT);
    gopro2->setMode(GPIODevice::OUTPUT);
	gopro1->write(false);
	gopro2->write(false);

	// Note: GoPro signals are only trigger signals
	usleep(500 * 1000); // wait 700ms

	// Set GoPro pins to floating
    gopro1->setMode(GPIODevice::INPUT);
    gopro2->setMode(GPIODevice::INPUT);

	gopro_enabled = true;
}

void Actuator_GoPro::disableGoPro(bool debug) {
	if (gopro_enabled) {
        if(isDebugMode && !debug) {
            return;
        }

        // Write "1" to output
        gopro1->setMode(GPIODevice::OUTPUT);
        gopro2->setMode(GPIODevice::OUTPUT);
        gopro1->write(false);
        gopro2->write(false);

        // Note: GoPro signals are only trigger signals
        usleep(2300 * 1000); // wait 2500ms

		gopro_enabled = false;
	}
}

void Actuator_GoPro::disable(bool debug)
{
    if(!isDebugMode || debug) {
        camsupply->write(true);
        lights1->write(false);
        lights2->write(false);
    }
}
