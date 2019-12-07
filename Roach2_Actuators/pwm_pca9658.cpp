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

	double temp = (25000000 / (4096 * PWM_FREQUENCY)); //KONTROLLIEREN
	temp = round(temp) - 1;
	write8(PRE_SCALE, (int8_t)temp);
}

void PWM_PCA985::drive()
{
	write8(MOT1_ON_H, 0b00010110); //Bitte checken
	write8(MOT2_ON_H, 0b00010110);	
	write8(MOT1_OFF_H, 0b00000000);
	write8(MOT2_OFF_H, 0b00000000);
	std::cout << "Setting PWM0 and PWM1 to: Drive!";
}

void PWM_PCA985::stop()
{
	write8(MOT1_ON_H, 0b00000000); //Bitte checken
	write8(MOT2_ON_H, 0b00000000);
	write8(MOT1_OFF_H, 0b00010000);
	write8(MOT2_OFF_H, 0b00010000); //bitte checken
	std::cout << "Setting PWM0 and PWM1 to: STOP!";
}

void PWM_PCA985::resetlaser()
{
	/*HIER KÖNNTE IHRE WERBUNG STEHEN*/
}



int PWM_PCA985::getI2CAddr()
{
	return 0;
}

void PWM_PCA985::enable()
{
	this->drive();
}

void PWM_PCA985::disable()
{
	this->stop();
}

int PWM_PCA985::getActutator_type()
{
	return ACTUATOR_LIST::ENGINE_PWM;
}


int PWM_PCA985::write8(int reg, int data) {
	int w8 = wiringPiI2CWriteReg8(this->deviceHandle, reg, data);
	return w8;//
}

int PWM_PCA985::read8(int reg) {
	//int readAcht = 0;  I2C_SMBUS_READ_BYTE_DATA(this->deviceHandle, (u_int8_t)reg);
	//std::cout << "read8 device handle: " << this->deviceHandle << std::endl;
	int res = wiringPiI2CReadReg8(this->deviceHandle, reg);
	return res;//readAcht;
}

int PWM_PCA985::write16(int reg, int data) {
	int w16 = wiringPiI2CWriteReg16(this->deviceHandle, reg, data);
	return w16;// 
}

int PWM_PCA985::read16(int reg) {
	int r16 = wiringPiI2CReadReg16(this->deviceHandle, reg);
	return r16;// 
}

int PWM_PCA985::i2cConnect(const int dev) {
	int fd = open("/dev/i2c-0", O_RDWR);
	int con = ioctl(fd, I2C_SLAVE, dev);
	return fd;
}
int PWM_PCA985::binaryToDecimal(int number)
{
	int num = number;
	int dec_value = 0;

	// Initializing base value to 1, i.e 2^0 
	int base = 1;

	int temp = num;
	while (temp) {
		int last_digit = temp % 10;
		temp = temp / 10;

		dec_value += last_digit * base;

		base = base * 2;
	}

	return dec_value;
}


int PWM_PCA985::simpleRead()
{
	int simpRead = wiringPiI2CRead(this->deviceHandle);// i2c_smbus_read_byte(this->deviceHandle);
	return simpRead; //
}

int PWM_PCA985::simpleWrite(int data)
{
	int sW = wiringPiI2CWrite(this->deviceHandle, data);
	return sW;// 
}
