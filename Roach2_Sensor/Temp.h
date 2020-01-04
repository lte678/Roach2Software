
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
// Hard coded settings
const int LM75B_DEVICE_ID = 0x48;

/* ##################################################################################### */
// Config register values
enum Setting_Temp {
	CONF_TEMP = 0b00000000, /*Standard einstellung*/
	TOS_TEMP = 0b1101001000000000, /*210 -> 105�C*, only 9 are significant*/
	THYST_TEMP = 0b1100100000000000 ,/*200 -> 100�C ... dummyvalue ;)*/
};

// Addresses of the 4 internal registers
enum LM758_REGISTER_t {
	TEMP_reg = 0b00000000 ,
	CONF_reg = 0b00000001,
	THYST_reg = 0b00000010,
	TOS_reg =  0b00000011,
};

/* ##################################################################################### */
class TEMP_LM75B : public Sensor {
public:
	explicit TEMP_LM75B(float updateFreq);
	~TEMP_LM75B() = default;

	// Inherited from Sensor base class
	void init() override;
	void update() override;
	Data* getData() override;
	int getI2CAddr() override;
	SensorType getSensorType() override;
private:
	double convertedMeasurement;
	unsigned long timeStamp = 0;
};

#endif //TEMP_LM75B