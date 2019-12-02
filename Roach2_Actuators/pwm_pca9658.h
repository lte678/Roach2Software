
/**
* @file pwm.h
* 12-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/

#ifndef PWM_PCA985_H
#define PWM_PCA985_H

#include "Actuator.h"
#include <wiringPiI2C.h>
#include <bitset>
#include <iostream>
#include <chrono>
#include "../Roach2_DataStore/data.h"

/* ##################################################################################### */
/* 0. Hard coded settings  */
const int PCA985_DEVICE_ID = 0x40; // in Schaltplan schauen
const int PWM_FREQUENCY = 100//  TESTVALUE!!!! IN Hz
/* ##################################################################################### */
/* 1. Enumerations */
enum Setting {
	DUMMY = 0b00000000, /*Standard einstellung*/
};
//SCHREIB HIER MEHR REIN
enum PCA985_REGISTER_t {
	MODE1 = 0b00000000,
    MODE2 = 0b00000001,
	SUBADR1 = 0b00000010,
	SUBADR2 = 0b00000011,
	SUBADR3 = 0b00000100,
	ALLCALLADR = 0b00000101,
	MOT1_ON_L =  0b00000110,
	MOT1_ON_H =  0b00000111,
	MOT1_OFF_L = 0b00001000,
	MOT1_OFF_H = 0b00001001,
	MOT2_ON_L = 0b00001010,
	MOT2_ON_H = 0b00001011,
	MOT2_OFF_L = 0b00001100,
	MOT2_OFF_H = 0b00001101, //----
	DIST2_ON_L = 0b00001110,
	DIST2_ON_H = 0b00001111,
	DIST2_OFF_L = 0b00010000,
	DIST2_OFF_H = 0b00010001,
	DIST3_ON_L  = 0b00010010,
	DIST3_ON_H  = 0b00010011,
	DIST3_OFF_L = 0b00010100,
	DIST3_OFF_H = 0b00010101,
	DIST4_ON_L  = 0b00010110,
	DIST4_ON_H  = 0b00010111,
	DIST4_OFF_L = 0b00011000,
	DIST4_OFF_H = 0b00011001,
	DIST5_ON_L  = 0b00011010,
	DIST5_ON_H  = 0b00011011,
	DIST5_OFF_L = 0b00011100,
	DIST5_OFF_H = 0b00011101,
	DIST6_ON_L  = 0b00011110,
	DIST6_ON_H  = 0b00011111,
	DIST6_OFF_L = 0b00100000,
	DIST6_OFF_H = 0b00100001,
	DIST7_ON_L  = 0b00100010,
	DIST7_ON_H  = 0b00100011,
	DIST7_OFF_L = 0b00100100,
	DIST7_OFF_H = 0b00100101,
	DIST8_ON_L  = 0b00100110,
	DIST8_ON_H  = 0b00100111,
	DIST8_OFF_L = 0b00101000,
	DIST8_OFF_H = 0b00101001,
	LED_ON_L    = 0b00101010,
	LED_ON_H    = 0b00101011,
	LED_OFF_L   = 0b00101100,
	LED_OFF_H   = 0b00101101,
	PRE_SCALE   = 0b11111110
};

/* ##################################################################################### */
/* 2. Class Definition */

class PWM_PCA985 : public Actuator {

public:
	PWM_PCA985();
	~PWM_PCA985();
	void init();
	void drive();
	void stop();
	void resetlaser();
	int getI2CAddr();
private:
	int freq = 0;
};

#endif //PWM_PCA985_H
