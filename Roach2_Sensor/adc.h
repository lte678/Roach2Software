/**
* @file adc.h
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef ADC_MCP3428_HEADER
#define ADC_MCP3428_HEADER

#include "Roach2_Sensor_Sensor.h"
#include "../Roach2_DataStore/data.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <iostream>
#include <chrono>
#include <string>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const int MCP3428_DEVICE_ID = 0x68; // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting {
	CONF_ADC1 = 0b10011000, /*CH1, 15 sps, conti, x1 gain*/
	CONF_ADC2 = 0b10111000, /*CH2, 15 sps, conti, x1 gain*/
    CONF_ADC3 = 0b11011000, /*CH3, 15 sps, conti, x1 gain*/
	CONF_ADC4 = 0b11111000 /*CH4, 15 sps, conti, x1 gain*/
};
//SCHREIB HIER MEHR REIN

/* ##################################################################################### */
/* 2. Class Definition */

class ADC_MCP3428 : public Sensor {
	/* Sensor class for MCP3428 */

public:
	ADC_MCP3428();
	~ADC_MCP3428();
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
	int getSensorType();
	//int logOvertemperature();
private:
	int measurement[4] = {};
	int ilauf = 1;
	int adress[4];
	double convertedMeasurement[4] = {};
	unsigned long timeStamp = 0;
	Data* data_obj;
};

#endif //ADC_MCP3428