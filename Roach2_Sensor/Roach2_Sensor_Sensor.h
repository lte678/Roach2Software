/**
* @file Roach2_Sensor_Sensor.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#ifndef Sensor_H
#define Sensor_H

#include "../Roach2_DataStore/data.h"
//#include "wiringPiSPI.h"
#include <wiringPiI2C.h>
//#include <wiringPi.h>
#include <bitset>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

enum class SensorType {
    TEMP_SENSOR = 0,
    IMU = 1,
    LASERDIST_0 = 2,
    LASERDIST_1 = 3,
    LASERDIST_2 = 4,
    LASERDIST_3 = 5,
    LASERDIST_4 = 6,
    LASERDIST_5 = 7,
    ADC = 8,
    ROT_ENC = 9,
    SYS_INFO = 10,
    OBC_SYS_INFO = 11,
    RCU_SYS_INFO = 12,
    RX_SIGNALS = 13
};

class Sensor
{
	/**
	* @brief The sensor parent class manages common functions for all Sensors
	*/
private:
    unsigned int sleepTime;
public:
    Sensor(float updateFreq); //Update frequency in Hz
    ~Sensor();
    void run(); // Each sensor has it's own update loop
    virtual void init() = 0;
    virtual void update() = 0;
    virtual Data* getData() = 0;
    virtual int getI2CAddr() = 0;

    /**
    * @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
    * @return SensorType sensor identifier
    */
    virtual SensorType getSensorType() = 0;
	// I2C sensor functions are placed here to avoid redefining them for most sensors
protected:
    int deviceHandle;
    int write8(int reg, int data);
    int read8(int reg);
    int write16(int reg, int data);
    int read16(int reg);
    int i2cConnect(int dev);
    int static binaryToDecimal(int number);
    int simpleRead();
    int simpleWrite(int data);
};

#endif //Sensor_H