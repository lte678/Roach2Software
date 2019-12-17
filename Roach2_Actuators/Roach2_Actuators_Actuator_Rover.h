#ifndef ACTUATOR_ROVER_H
#define ACTUATOR_ROVER_H

#include "Actuator.h"
class Actuator_Rover :
	public Actuator
{
public:
	Actuator_Rover();
	~Actuator_Rover();
	int getActutator_type();
	void enable();
	void disable();
private:
	int fd_actuator_rover;
protected:

};

#endif