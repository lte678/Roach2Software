//
// Created by leon on 13.01.20.
//

#include "I2CDevice.h"


#ifndef LOCAL_DEV
I2CDevice::I2CDevice(unsigned int address) {
    deviceValid = false;
    deviceAddress = address;
    deviceHandle = open("/dev/i2c-0", O_RDWR);
    if(deviceHandle != -1) {
        if(ioctl(deviceHandle, I2C_SLAVE, deviceAddress) != -1) {
            deviceValid = true;
        }
    }
}

I2CDevice::~I2CDevice() {
    if(deviceValid) {
        close(deviceHandle);
    }
}

int I2CDevice::simpleRead()
{
	return wiringPiI2CRead(deviceHandle);
}

int I2CDevice::simpleWrite(int data)
{
	return wiringPiI2CWrite(deviceHandle, data);
}

int I2CDevice::write8(int reg, int data) {
    return wiringPiI2CWriteReg8(deviceHandle, reg, data);
}

int I2CDevice::read8(int reg) {
    return wiringPiI2CReadReg8(deviceHandle, reg);
}

int I2CDevice::write16(int reg, int data) {
    return wiringPiI2CWriteReg16(deviceHandle, reg, data);
}

int I2CDevice::read16(int reg) {
    return wiringPiI2CReadReg16(deviceHandle, reg);
}

#else
// Dummy interfaces

I2CDevice::I2CDevice(unsigned int address) {
    deviceValid = false;
}

I2CDevice::~I2CDevice() {

}

int I2CDevice::simpleRead() {
    return 0;
}

int I2CDevice::simpleWrite(int data) {
    return 0;
}

int I2CDevice::write8(int reg, int data) {
    return 0;
}

int I2CDevice::read8(int reg) {
    return 0;
}

int I2CDevice::write16(int reg, int data) {
    return 0;
}

int I2CDevice::read16(int reg) {
    return 0;
}
#endif