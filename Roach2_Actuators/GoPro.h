#ifndef _Actuator_GoPro_h_
#define _Actuator_GoPro_h_

#include <iostream>

#include "Actuator.h"
#include "../Roach2_Hardware/GPIODevice.h"

class Actuator_GoPro : public Actuator
{
private:
    GPIODevice* gopro1;
    GPIODevice* gopro2;
    GPIODevice* camsupply;
    GPIODevice* lights1;
    GPIODevice* lights2;
    bool gopro_enabled;
public:
	Actuator_GoPro();
	~Actuator_GoPro();
	int getActuatorType() override;
	void enable(bool debug) override;
	void disable(bool debug) override;
	void disableGoPro(bool debug);
};

#endif
