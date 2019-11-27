#pragma once
#ifndef _Actuator_GoPro_h_
#define _Actuator_GoPro_h_

#include "Actuator.h"

class Actuator_GoPro : public Actuator
{
public:
	Actuator_GoPro();
	~Actuator_GoPro();
	int getActutator_type();
	void enable();
	void disable();
private:
	int fd_gopro1;
	int fd_gopro2;
	int fd_camsupply;
protected:

};

#endif
