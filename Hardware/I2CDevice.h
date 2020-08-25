//
// Created by leon on 13.01.20.
//

#ifndef ROACH2_SOFTWARE_I2CDEVICE_H
#define ROACH2_SOFTWARE_I2CDEVICE_H

#ifndef LOCAL_DEV
#include <wiringPiI2C.h>
#endif

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class I2CDevice {
private:
    unsigned int deviceAddress;
    int deviceHandle;
    bool deviceValid;
public:
    explicit I2CDevice(unsigned int address);
    ~I2CDevice();
    bool isValid() { return deviceValid; };

    int simpleRead();
    int simpleWrite(int data);
    int write8(int reg, int data);
    int read8(int reg);
    int write16(int reg, int data);
    int read16(int reg);
};


#endif //ROACH2_SOFTWARE_I2CDEVICE_H
