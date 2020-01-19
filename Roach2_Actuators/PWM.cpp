#include "PWM.h"

PWM_PCA985::PWM_PCA985()
{
    std::cout << "[Actuator|PWM] Initializing" << std::endl;
	pwmDriver = new I2CDevice(PCA985_DEVICE_ID);
	if(!pwmDriver->isValid()) {
	    std::cout << "[Actuator|PWM] Failed to create i2c device!" << std::endl;
	}
	init();
}

PWM_PCA985::~PWM_PCA985()
{
    delete pwmDriver;
}

void PWM_PCA985::init()
{
    // Wir setzen die maximale PWM Frequenz, die ist für uns geeignet
    pwmDriver->write8(PRE_SCALE, 0x03); // 0x03 ist minimaler Faktor
    pwmDriver->write8(MODE1, 0x01); // Exit sleep mode
}

void PWM_PCA985::enableLEDs(bool debug) {
	// LEDS on
	if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|PWM] Enabling LEDs" << std::endl;

        pwmDriver->write8(0x2B, 0x10);
        pwmDriver->write8(0x2D, 0x00);
	}

}

void PWM_PCA985::disableLEDs(bool debug) {
	// LEDS off
    if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|PWM] Disabling LEDs" << std::endl;

        pwmDriver->write8(0x2B, 0x00);
        pwmDriver->write8(0x2D, 0x10);
    }
}

void PWM_PCA985::drive(bool debug)
{
    if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|PWM] Enabling motor" << std::endl;

	    //pwmDriver->write8(0x07, 0x10);
	    //pwmDriver->write8(0x09, 0x00);

	    int onSteps = (unsigned int)((1.0f - pwmMotorOnPercentage) * (float)0xFFF); // Maximalwert 4095;
        pwmDriver->write8(MOTOR_ON_L, onSteps & 0xFF);
        pwmDriver->write8(MOTOR_ON_H, (onSteps & 0xF00) >> 8);
        pwmDriver->write8(MOTOR_OFF_L, 0x00);
        pwmDriver->write8(MOTOR_OFF_H, 0x00);
    }
}

void PWM_PCA985::stop(bool debug)
{
    if((!isDebugMode && !debug) || (debug && isDebugMode)) {
        std::cout << "[Actuator|PWM] Disabling motor" << std::endl;

        pwmDriver->write8(0x07, 0x00);
        pwmDriver->write8(0x09, 0x10);
    }

	/*write8(MOT1_ON_H, 0b00000000); //Bitte checken
	write8(MOT2_ON_H, 0b00000000);
	write8(MOT1_OFF_H, 0b00010000);
	write8(MOT2_OFF_H, 0b00010000); //bitte checken
	std::cout << "Setting PWM0 and PWM1 to: STOP!";
	*/
}

void PWM_PCA985::resetlaser()
{
	/*HIER K�NNTE IHRE WERBUNG STEHEN*/
}

void PWM_PCA985::enable(bool debug)
{
	drive(debug);
}

void PWM_PCA985::disable(bool debug)
{
	stop(debug);
}

int PWM_PCA985::getActuatorType()
{
	return ACTUATOR_LIST::ENGINE_PWM;
}


