

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

/* ##################################################################################### */
/* 0. Hard coded settings  */
const long CALIBRATION_TIMEOUT_SECONDS = 60;
const int MCP3428_DEVICE_ID = 0x68; // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting {
	CONF_ADC = 0b10011000 /*CH1, 15 sps, conti, x1 gain*/
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
	//int logOvertemperature();
private:
	int deviceHandle = 0;
	int measurement = 0;
	unsigned long timeStamp = 0;
	Data* data_obj;
};

#endif //ADC_MCP3428