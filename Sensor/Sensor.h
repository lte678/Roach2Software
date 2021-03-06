/**
* @file Sensor.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#ifndef Sensor_H
#define Sensor_H

#ifndef LOCAL_DEV
#include <wiringPiI2C.h>
#endif

#include "../DataStore/data.h"

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

#include <memory>

enum class SensorType {
        TEMP_SENSOR = 0,
        IMU = 1,
        LASERDIST_0 = 2, // Backplane rover
        LASERDIST_1 = 3, // Backplane rover
        LASERDIST_2 = 4, // Bottom rover
        LASERDIST_3 = 5, // Bottom rover
        LASERDIST_4 = 6, // Bottom rover
        LASERDIST_5 = 7, // Bottom rover
        ADC = 8,
        ROT_ENC = 9,
        SYS_INFO = 10,
        OBC_SYS_INFO = 11,
        RCU_SYS_INFO = 12,
        LASERDIST_6 = 13, // Front rover
        LASERDIST_7 = 14, // Front rover
        LASERDIST_8 = 15, // Top rover
        RX_SIGNALS = 16
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
    virtual std::unique_ptr<Data> getData() = 0;
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
    void readBlock(char *buffer, int n);
};

#endif //Sensor_H