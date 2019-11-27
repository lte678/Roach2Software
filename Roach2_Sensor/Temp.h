
/**
* @file Temp.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef TEMP_LM75B_H
#define TEMP_LM75B_H

#include "Roach2_Sensor_Sensor.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <iostream>
#include <chrono>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const long CALIBRATION_TIMEOUT_SECONDS = 60;
const int LM75B_DEVICE_ID = 0x48; // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting_Temp {
	CONF_TEMP = 0b00000000, /*Standard einstellung*/
	TOS_TEMP = 0b1101001000000000, /*210 -> 105°C*, only 9 are significant*/
	THYST_TEMP = 0b1100100000000000 ,/*200 -> 100°C ... dummyvalue ;)*/
};
//SCHREIB HIER MEHR REIN
enum LM758_REGISTER_t {
	TEMP_reg = 0b00000000 ,
	CONF_reg = 0b00000001,
	THYST_reg = 0b00000010,
	TOS_reg =  0b00000011,
	/* change the pointer to Temperature, or conf*/
};

/* ##################################################################################### */
/* 2. Class Definition */

class TEMP_LM75B : public Sensor {
	/* Sensor class for LM75B */
	
public:
	TEMP_LM75B();
	~TEMP_LM75B();
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
	int getSensorType();
private:
	void tempConfig(); // Separate config function, to be called in the init()
	int measurement = 0;
	double convertedMeasurement = 0.0;
	unsigned long timeStamp = 0;
};

#endif //TEMP_LM75B