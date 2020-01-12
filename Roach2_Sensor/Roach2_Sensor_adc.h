/**
* @file adc.h
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef ADC_MCP3428_HEADER
#define ADC_MCP3428_HEADER

#ifndef LOCAL_DEV
#include <wiringPiI2C.h>
#endif

#include "Roach2_Sensor_Sensor.h"
#include "../Roach2_DataStore/data.h"

#include <bitset>
#include <iostream>
#include <chrono>
#include <string>

/* ##################################################################################### */
/* 0. Hard coded settings  */
const int MCP3428_DEVICE_ID = 0x68; // in Schaltplan schauen

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting_Adc {
	CONF_ADC1 = 0b10011000, /*CH1, 15 sps, conti, x1 gain*/
	CONF_ADC2 = 0b10111000, /*CH2, 15 sps, conti, x1 gain*/
    CONF_ADC3 = 0b11011000, /*CH3, 15 sps, conti, x1 gain*/
	CONF_ADC4 = 0b11111000 /*CH4, 15 sps, conti, x1 gain*/
};
//SCHREIB HIER MEHR REIN

/* ##################################################################################### */
/* 2. Class Definition */

class ADC_MCP3428 : public Sensor {
public:
	explicit ADC_MCP3428(float updateFreq);
	~ADC_MCP3428();
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
    SensorType getSensorType();
private:
	int measurement[4] = {};
	int ilauf = 1;
	int adress[4];
	double convertedMeasurement[4] = {};
	unsigned long timeStamp = 0;
	Data* data_obj;
};

#endif //ADC_MCP3428