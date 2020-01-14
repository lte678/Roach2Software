
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

#include "Roach2_Sensor_Sensor.h"

#include <bitset>
#include <iostream>
#include <chrono>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const int AS5601_DEVICE_ID = 0x36; // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting_Rot {
	CONF_ROT = 0b00000000, /*Standard einstellung*/
};
//SCHREIB HIER MEHR REIN
enum AS5601_REGISTER_t {
	RAW1_reg = 0x0C,
	RAW2_reg = 0x0D,
	ANG_reg = 0x0E,
	ANG2_reg = 0x0F
};

/* ##################################################################################### */
/* 2. Class Definition */

class ROT_AS5601 : public Sensor {

public:
	explicit ROT_AS5601(float updateFreq);
	~ROT_AS5601();
	void init();
	void update();
    std::unique_ptr<Data> getData();
	int getI2CAddr();
    SensorType getSensorType();
private:
	int measurement[2] = {};
	unsigned long timeStamp = 0;
};

#endif //ROT_AS5601