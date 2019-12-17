#include "Roach2_Sensor_Sensor.h"
// Ziel: Oberklasse Sensor. Unterklassen der einzelnen sensoren übergeben Oberklasse Sensor alle daten und diese gibt sie gesammelt weiter.

Sensor::Sensor(){
};

Sensor::~Sensor(){
	
}

int Sensor::write8(int reg, int data){
	int w8 = wiringPiI2CWriteReg8(this->deviceHandle, reg, data);
	return w8;//
}

int Sensor::read8(int reg){
	//int readAcht = 0;  I2C_SMBUS_READ_BYTE_DATA(this->deviceHandle, (u_int8_t)reg);
	//std::cout << "read8 device handle: " << this->deviceHandle << std::endl;
	int res = wiringPiI2CReadReg8(this->deviceHandle, reg);
	return res;//readAcht;
}

int Sensor::write16(int reg, int data){
	int w16 = wiringPiI2CWriteReg16(this->deviceHandle, reg, data);
	return w16;// 
}

int Sensor::read16(int reg){
	int r16 = wiringPiI2CReadReg16(this->deviceHandle, reg);
	return r16;// 
}

int Sensor::i2cConnect(const int dev){
	int fd = open("/dev/i2c-0", O_RDWR);
	int con = ioctl(fd, I2C_SLAVE, dev);
	return fd;
}
int Sensor::binaryToDecimal(int number)
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


int Sensor::simpleRead()
{
	int simpRead = wiringPiI2CRead(this->deviceHandle);// i2c_smbus_read_byte(this->deviceHandle);
	return simpRead; //
}

int Sensor::simpleWrite(int data)
{
	int sW = wiringPiI2CWrite(this->deviceHandle, data);
	return sW;// 
}