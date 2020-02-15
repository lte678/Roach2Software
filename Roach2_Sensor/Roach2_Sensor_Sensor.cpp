#include "Roach2_Sensor_Sensor.h"
// Ziel: Oberklasse Sensor. Unterklassen der einzelnen sensoren �bergeben Oberklasse Sensor alle daten und diese gibt sie gesammelt weiter.

Sensor::Sensor(float updateFreq) {
    sleepTime = (unsigned int)(1e6 / updateFreq);
}

Sensor::~Sensor(){
	
}

void Sensor::run() {
    while(true) {
        update();
        usleep(sleepTime);
    }
}

int Sensor::write8(int reg, int data){
#ifndef LOCAL_DEV
	int w8 = wiringPiI2CWriteReg8(this->deviceHandle, reg, data);
#else
	int w8 = 0;
#endif
	return w8;//
}

int Sensor::read8(int reg){
#ifndef LOCAL_DEV
	int res = wiringPiI2CReadReg8(this->deviceHandle, reg);
#else
    int res = 0;
#endif
	return res;
}

int Sensor::write16(int reg, int data){
#ifndef LOCAL_DEV
	int w16 = wiringPiI2CWriteReg16(this->deviceHandle, reg, data);
#else
    int w16 = 0;
#endif
	return w16;// 
}

int Sensor::read16(int reg){
#ifndef LOCAL_DEV
	int r16 = wiringPiI2CReadReg16(this->deviceHandle, reg);
#else
    int r16 = 0;
#endif
	return r16;// 
}

int Sensor::i2cConnect(const int dev){
#ifndef LOCAL_DEV
	int fd = open("/dev/i2c-0", O_RDWR);
	int con = ioctl(fd, I2C_SLAVE, dev);
#else
	int fd = 0;
#endif
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
#ifndef LOCAL_DEV
	int simpRead = wiringPiI2CRead(this->deviceHandle);// i2c_smbus_read_byte(this->deviceHandle);
#else
    int simpRead = 0;
#endif
	return simpRead; //
}

int Sensor::simpleWrite(int data)
{
#ifndef LOCAL_DEV
	int sW = wiringPiI2CWrite(this->deviceHandle, data);
#else
	int sW = 0;
#endif
	return sW;// 
}

void Sensor::readBlock(char *buffer, int n) {
    if(deviceHandle) {
        read(deviceHandle, buffer, n);
    }
}
