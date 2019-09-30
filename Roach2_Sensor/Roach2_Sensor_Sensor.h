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
#include "wiringPiSPI.h"
#include "wiringPiI2C.h"
#include <bitset>
#include <iostream>
#include <chrono>

class Sensor
{
	/**
	* @brief The sensor parent class manages common functions for all Sensors
	*/
	
	public:
		Sensor();
		~Sensor();
		virtual void init() = 0;
		virtual void update() = 0;
		virtual Data* getData() = 0;
		virtual int getI2CAddr() = 0;
	// I2C sensor functions are placed here to avoid redefining them for most sensors
	protected:
		int deviceHandle;
		int write8(int reg, int data);
		int read8(int reg);
		int write16(int reg, int data);
		int read16(int reg);
		int i2cConnect(int dev);
		int binaryToDecimal(int number);
		int simpleRead();
		int simpleWrite(int data);
};

#endif //Sensor_H