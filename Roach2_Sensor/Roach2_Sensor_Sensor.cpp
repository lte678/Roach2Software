#include "Roach2_Sensor_Sensor.h"
// Ziel: Oberklasse Sensor. Unterklassen der einzelnen sensoren übergeben Oberklasse Sensor alle daten und diese gibt sie gesammelt weiter.

Sensor::Sensor(){
};

Sensor::~Sensor(){
	
}

int Sensor::write8(int reg, int data){
	return wiringPiI2CWriteReg8(this->deviceHandle, reg, data);
}

int Sensor::read8(int reg){
	return wiringPiI2CReadReg8(this->deviceHandle, reg);
}

int Sensor::write16(int reg, int data){
	return wiringPiI2CWriteReg16(this->deviceHandle, reg, data);
}

int Sensor::read16(int reg){
	return wiringPiI2CReadReg16(this->deviceHandle, reg);
}

int Sensor::i2cConnect(int dev){
	return wiringPiI2CSetup(dev);
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
	return wiringPiI2CRead(this->deviceHandle);
}

int Sensor::simpleWrite(int data)
{
	return wiringPiI2CWrite(this->deviceHandle, data);
}