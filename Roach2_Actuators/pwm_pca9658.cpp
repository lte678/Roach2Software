#include "pwm_pca9658.h"

PWM_PCA985::PWM_PCA985()
{
	deviceHandle = this->i2cConnect(PCA985_DEVICE_ID);
}

PWM_PCA985::~PWM_PCA985()
{
}

void PWM_PCA985::init()
{

	freq = (25000000 / (4096 * PWM_FREQUENCY)); //KONTROLLIEREN
	freq = freq / 2 + freq % 2;
	freq = freq - 1;
	write8(PRE_SCALE, freq);
}

void PWM_PCA985::drive()
{
	write8(MOT1_ON_H, 0b00000000); //Bitte checken
	write8(MOT2_ON_H, 0b00000000);		
}

void PWM_PCA985::stop()
{
	write8(MOT1_OFF_H, 0b00010000);
	write8(MOT2_OFF_H, 0b00010000); //bitte checken
}

void PWM_PCA985::resetlaser()
{
	/*HIER KÖNNTE IHRE WERBUNG STEHEN*/
}



int PWM_PCA985::getI2CAddr()
{
	return 0;
}
