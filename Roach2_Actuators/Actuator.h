#pragma once
#ifndef _Actuator_h_
#define _Actuator_h_

class Actuator
{
public:
	Actuator();
	~Actuator();
	int getState();
	void setState(int state);
	void enable();
	void disable();
	void enableDebugMode();
	void disableDebugMode();

private:
	bool isEnabled;
	bool isDebugMode;

protected:
	int state;
	void trigger();

};

#endif
