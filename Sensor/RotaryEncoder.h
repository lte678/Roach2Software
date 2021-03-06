
/**
* @file rotEnc.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef ROT_AS5601_H
#define ROT_AS5601_H

#ifndef LOCAL_DEV
#include <wiringPiI2C.h>
#endif

#include "Sensor.h"

#include <bitset>
#include <iostream>
#include <chrono>

/* ##################################################################################### */
/* 0. Hard coded settings  */
static const int AS5601_DEVICE_ID = 0x36; // in Schaltplan schauen
static const float PRESCALER = 360.0f / 0xFFF;

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting_Rot {
	CONF_ROT = 0b00000000, /*Standard einstellung*/
};
//SCHREIB HIER MEHR REIN
enum AS5601_REGISTER_t {
	RAW_REG = 0x0C,
	ANG_REG = 0x0E,
};

/* ##################################################################################### */
/* 2. Class Definition */

class ROT_AS5601 : public Sensor {

public:
	explicit ROT_AS5601(float updateFreq);
	~ROT_AS5601();
	void init();
	void update();
	float getAngle();
    std::unique_ptr<Data> getData();
	int getI2CAddr();
    SensorType getSensorType();
    //for Test-Purpose
    bool logData(float angle, float rot);

private:
	float angle;
	float startAngle;
	float rotationRate;
	unsigned int systemMillis;


};

#endif //ROT_AS5601