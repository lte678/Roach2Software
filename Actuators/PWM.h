
/**
* @file pwm.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef PWM_PCA985_H
#define PWM_PCA985_H

#include "Actuator.h"
#include "../DataStore/data.h"
#include "../Hardware/I2CDevice.h"

#include <cmath>

const int PCA985_DEVICE_ID = 0x70; // in Schaltplan schauen
const int PWM_FREQUENCY = 100; //  TESTVALUE!!!! IN Hz

enum PCA985_REGISTER_t {
	MODE1 = 0x00,
    MODE2 = 0x01,
    MOTOR_ON_L = 0x06,
	MOTOR_ON_H  = 0x07,
	MOTOR_OFF_L = 0x08,
	MOTOR_OFF_H = 0x09,
	PRE_SCALE   = 0xFE
};

class PWM_PCA985 : public Actuator {
private:
    I2CDevice* pwmDriver;
    constexpr static float pwmMotorOnPercentage = 0.5f;
public:
	PWM_PCA985();
	~PWM_PCA985();
	void init();
	void drive(bool debug = false);
	void stop(bool debug = false);
	void resetlaser();
	void enable(bool debug) override;
	void disable(bool debug) override;
	int getActuatorType() override;
	void enableLEDs(bool debug);
	void disableLEDs(bool debug);
};

#endif //PWM_PCA985_H
