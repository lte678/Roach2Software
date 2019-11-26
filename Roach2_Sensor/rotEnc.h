
/**
* @file rotEnc.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef ROT_AS5601_H
#define ROT_AS5601_H

#include "Roach2_Sensor_Sensor.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <iostream>
#include <chrono>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const int AS5601_DEVICE_ID = 0x36 // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting {
	CONF_ROT = 0b00000000, /*Standard einstellung*/
};
//SCHREIB HIER MEHR REIN
enum AS5601_REGISTER_t {
	RAW1_reg = 0x0C,
	RAW2_reg = 0x0D,
	ANG_reg = 0x0E,
	ANG2_reg = 0x0F
	/* change the pointer to Temperature, or conf*/
};

/* ##################################################################################### */
/* 2. Class Definition */

class ROT_AS5601 : public Sensor {
	/* Sensor class for LM75B */

public:
	ROT_AS5601();
	~ROT_AS5601();
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
	int getSensorType();
	//int logOvertemperature();
private:
	int measurement[2] = {};
	//double convertedMeasurement[2] = {};
	unsigned long timeStamp = 0;
};

#endif //ROT_AS5601