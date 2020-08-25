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
#include <linux/i2c-dev.h>
#endif

#include "Sensor.h"
#include "../DataStore/data.h"

#include <bitset>
#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>

/* ##################################################################################### */
/* 0. Hard coded settings  */
extern const int MCP3428_DEVICE_ID; // in Schaltplan schauen
extern const float LSB_SCALING;
extern const int MAX_READ_ATTEMPTS;

/* ##################################################################################### */
/* 1. Enumerations */
enum Setting_Adc {
	CONF_ADC1 = 0b10000000, /*CH1, 15 sps, conti, x1 gain*/
	CONF_ADC2 = 0b10100000, /*CH2, 15 sps, conti, x1 gain*/
    CONF_ADC3 = 0b11000000, /*CH3, 15 sps, conti, x1 gain*/
	CONF_ADC4 = 0b11100000 /*CH4, 15 sps, conti, x1 gain*/
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
    std::unique_ptr<Data> getData();
	int getI2CAddr();
    SensorType getSensorType();
private:
	unsigned char chSetting[4];
	std::mutex dataLock;
	double convertedMeasurement[4]{};
};

#endif //ADC_MCP3428