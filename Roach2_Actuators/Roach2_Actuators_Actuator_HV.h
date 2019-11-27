#pragma once
#ifndef _Actuator_HV_h_
#define _Actuator_HV_h_

#include "Actuator.h"

class Actuator_HV : public Actuator
{
public:
	Actuator_HV();
	~Actuator_HV();
	int getActutator_type();
	void enable();
	void disable();
private:
	int fd_hv_left;
	int fd_hv_right;
protected:

};

#endif 
