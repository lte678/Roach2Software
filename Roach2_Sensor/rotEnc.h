#pragma once


#pragma once

/**
* @file adc.h
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef rotEnc_LS7366R
#define rotEnc_LS7366R
#include "Roach2_Sensor_Sensor.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <iostream>
#include <chrono>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const long CALIBRATION_TIMEOUT_SECONDS = 60;
const int LS7366R_CHANNEL = 0; // in Schaltplan schauen
const int speed = 500000
/* ##################################################################################### */
/* 1. Enumerations */
enum Setting {
	CONF_ADC = 0b10011000 /*CH1, 15 sps, conti, x1 gain*/
};
//SCHREIB HIER MEHR REIN

/* ##################################################################################### */
/* 2. Class Definition */

class rotEnc_LS7366R : public Sensor {
	/* Sensor class for MCP3428 */

public:
	rotEnc_LS7366R();
	rotEnc_LS7366R();
	void init();
	void update();
	Data getData();
	int getI2CAddr();
	//int logOvertemperature();
private:
	int deviceHandle = 0;
	int measurement = 0;
	unsigned long timeStamp = 0;
};

#endif //ADC_MCP3428