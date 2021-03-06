//
// Created by Christian Molliere on 2019-04-20.
// Modified by Alexander Waigum on 2019-07-09.
//

#ifndef IMU_TEST_BNO055_H
#define IMU_TEST_BNO055_H

#ifndef LOCAL_DEV
#include <wiringPiI2C.h>
#endif

#include "../DataStore/data.h"
#include "Sensor.h"

#include <bitset>
#include <iostream>
#include <chrono>
#include <memory>

/* ##################################################################################### */
/* 2. Class Definition */

class BNO055_IMU : public Sensor {
    /* Sensor class for BNO055 */
    private:
		const long CALIBRATION_TIMEOUT_SECONDS = 60;
		const int BNO055_DEVICE_ID = 0x28;
        //int deviceHandle = 0;
		double acc[3];
		double mag[3];
		double gyr[3];
		double quat[4];
		double rot[3];
		double grav[3];
		double temp;
		/* ##################################################################################### */
/* 1. Enumerations */
    enum BNO055_OPERATION_MODES_t {
        // non-fusion modes
        ACC_ONLY_CONF = 0b0000001,
        MAG_ONLY_CONF = 0b0000010,
        GYRO_ONLY_CONF = 0b0000011,
        ACC_MAG_CONF = 0b0000100,
        ACC_GYRO_CONF = 0b0000101,
        MAG_GYRO_CONF = 0b0000110,
        ACC_MAG_GYRO_CONF = 0b0000111, // <- DE (alternative) WEY
        // fusion modes
        IMU_CONF_CONF = 0b0001000,
        COMPASS_CONF = 0b0001001,
        M4G_CONF = 0b0001010,
        NDOF_FMC_OFF_CONF = 0b0001011,
        NDOF_CONF = 0b0001100 //<- DE WAY
    };

    enum BNO055REGISTERt {
        /* Page id register definition */
        PAGE_ID_ADDR = 0X07,
        /* PAGE0 REGISTER DEFINITION START*/
        CHIP_ID_ADDR = 0x00,
        ACCEL_REV_ID_ADDR = 0x01,
        MAG_REV_ID_ADDR = 0x02,
        GYRO_REV_ID_ADDR = 0x03,
        SW_REV_ID_LSB_ADDR = 0x04,
        SW_REV_ID_MSB_ADDR = 0x05,
        BL_REV_ID_ADDR = 0X06,
        /* Accel data register */
        ACCEL_DATA_X_LSB_ADDR = 0X08,
        ACCEL_DATA_X_MSB_ADDR = 0X09,
        ACCEL_DATA_Y_LSB_ADDR = 0X0A,
        ACCEL_DATA_Y_MSB_ADDR = 0X0B,
        ACCEL_DATA_Z_LSB_ADDR = 0X0C,
        ACCEL_DATA_Z_MSB_ADDR = 0X0D,
        /* Mag data register */
        MAG_DATA_X_LSB_ADDR = 0X0E,
        MAG_DATA_X_MSB_ADDR = 0X0F,
        MAG_DATA_Y_LSB_ADDR = 0X10,
        MAG_DATA_Y_MSB_ADDR = 0X11,
        MAG_DATA_Z_LSB_ADDR = 0X12,
        MAG_DATA_Z_MSB_ADDR = 0X13,
        /* Gyro data registers */
        GYRO_DATA_X_LSB_ADDR = 0X14,
        GYRO_DATA_X_MSB_ADDR = 0X15,
        GYRO_DATA_Y_LSB_ADDR = 0X16,
        GYRO_DATA_Y_MSB_ADDR = 0X17,
        GYRO_DATA_Z_LSB_ADDR = 0X18,
        GYRO_DATA_Z_MSB_ADDR = 0X19,
        /* Euler data registers */
        EULER_H_LSB_ADDR = 0X1A,
        EULER_H_MSB_ADDR = 0X1B,
        EULER_R_LSB_ADDR = 0X1C,
        EULER_R_MSB_ADDR = 0X1D,
        EULER_P_LSB_ADDR = 0X1E,
        EULER_P_MSB_ADDR = 0X1F,
        /* Quaternion data registers */
        QUATERNION_DATA_W_LSB_ADDR = 0X20,
        QUATERNION_DATA_W_MSB_ADDR = 0X21,
        QUATERNION_DATA_X_LSB_ADDR = 0X22,
        QUATERNION_DATA_X_MSB_ADDR = 0X23,
        QUATERNION_DATA_Y_LSB_ADDR = 0X24,
        QUATERNION_DATA_Y_MSB_ADDR = 0X25,
        QUATERNION_DATA_Z_LSB_ADDR = 0X26,
        QUATERNION_DATA_Z_MSB_ADDR = 0X27,
        /* Linear acceleration data registers */
        LINEAR_ACCEL_DATA_X_LSB_ADDR = 0X28,
        LINEAR_ACCEL_DATA_X_MSB_ADDR = 0X29,
        LINEAR_ACCEL_DATA_Y_LSB_ADDR = 0X2A,
        LINEAR_ACCEL_DATA_Y_MSB_ADDR = 0X2B,
        LINEAR_ACCEL_DATA_Z_LSB_ADDR = 0X2C,
        LINEAR_ACCEL_DATA_Z_MSB_ADDR = 0X2D,
        /* Gravity data registers */
        GRAVITY_DATA_X_LSB_ADDR = 0X2E,
        GRAVITY_DATA_X_MSB_ADDR = 0X2F,
        GRAVITY_DATA_Y_LSB_ADDR = 0X30,
        GRAVITY_DATA_Y_MSB_ADDR = 0X31,
        GRAVITY_DATA_Z_LSB_ADDR = 0X32,
        GRAVITY_DATA_Z_MSB_ADDR = 0X33,
        /* Temperature data register */
        TEMP_ADDR = 0X34,
        /* Status registers */
        CALIB_STAT_ADDR = 0X35,
        SELFTEST_RESULT_ADDR = 0X36,
        INTR_STAT_ADDR = 0X37,
        SYS_CLK_STAT_ADDR = 0X38,
        SYS_STAT_ADDR = 0X39,
        SYS_ERR_ADDR = 0X3A,
        /* Unit selection register */
        UNIT_SEL_ADDR = 0X3B,
        DATA_SELECT_ADDR = 0X3C,
        /* Mode registers */
        OPR_MODE_ADDR = 0X3D,
        PWR_MODE_ADDR = 0X3E,
        SYS_TRIGGER_ADDR = 0X3F,
        TEMP_SOURCE_ADDR = 0X40,
        /* Axis remap registers */
        AXIS_MAP_CONFIG_ADDR = 0X41,
        AXIS_MAP_SIGN_ADDR = 0X42,
        /* SIC registers */
        SIC_MATRIX_0_LSB_ADDR = 0X43,
        SIC_MATRIX_0_MSB_ADDR = 0X44,
        SIC_MATRIX_1_LSB_ADDR = 0X45,
        SIC_MATRIX_1_MSB_ADDR = 0X46,
        SIC_MATRIX_2_LSB_ADDR = 0X47,
        SIC_MATRIX_2_MSB_ADDR = 0X48,
        SIC_MATRIX_3_LSB_ADDR = 0X49,
        SIC_MATRIX_3_MSB_ADDR = 0X4A,
        SIC_MATRIX_4_LSB_ADDR = 0X4B,
        SIC_MATRIX_4_MSB_ADDR = 0X4C,
        SIC_MATRIX_5_LSB_ADDR = 0X4D,
        SIC_MATRIX_5_MSB_ADDR = 0X4E,
        SIC_MATRIX_6_LSB_ADDR = 0X4F,
        SIC_MATRIX_6_MSB_ADDR = 0X50,
        SIC_MATRIX_7_LSB_ADDR = 0X51,
        SIC_MATRIX_7_MSB_ADDR = 0X52,
        SIC_MATRIX_8_LSB_ADDR = 0X53,
        SIC_MATRIX_8_MSB_ADDR = 0X54,
        /* Accelerometer Offset registers */
        ACCEL_OFFSET_X_LSB_ADDR = 0X55,
        ACCEL_OFFSET_X_MSB_ADDR = 0X56,
        ACCEL_OFFSET_Y_LSB_ADDR = 0X57,
        ACCEL_OFFSET_Y_MSB_ADDR = 0X58,
        ACCEL_OFFSET_Z_LSB_ADDR = 0X59,
        ACCEL_OFFSET_Z_MSB_ADDR = 0X5A,
        /* Magnetometer Offset registers */
        MAG_OFFSET_X_LSB_ADDR = 0X5B,
        MAG_OFFSET_X_MSB_ADDR = 0X5C,
        MAG_OFFSET_Y_LSB_ADDR = 0X5D,
        MAG_OFFSET_Y_MSB_ADDR = 0X5E,
        MAG_OFFSET_Z_LSB_ADDR = 0X5F,
        MAG_OFFSET_Z_MSB_ADDR = 0X60,
        /* Gyroscope Offset register s*/
        GYRO_OFFSET_X_LSB_ADDR = 0X61,
        GYRO_OFFSET_X_MSB_ADDR = 0X62,
        GYRO_OFFSET_Y_LSB_ADDR = 0X63,
        GYRO_OFFSET_Y_MSB_ADDR = 0X64,
        GYRO_OFFSET_Z_LSB_ADDR = 0X65,
        GYRO_OFFSET_Z_MSB_ADDR = 0X66,
        /* Radius registers */
        ACCEL_RADIUS_LSB_ADDR = 0X67,
        ACCEL_RADIUS_MSB_ADDR = 0X68,
        MAG_RADIUS_LSB_ADDR = 0X69,
        MAG_RADIUS_MSB_ADDR = 0X6A
    };
    /* Registerpage 1 (in reg 0x07 0x01 schreiben)*/
    enum BNO055REGISTERt1
    {
        /*Verschiedene Configregister*/
        AccConf = 0x08,
        MagConf = 0x09,
        GyrConf0 = 0x0A,
        GyrConf1 = 0x0B,
        AccSleepConfig = 0x0C,
        GyrSleepConfig = 0x0D,
        IntMask = 0x0F, /* Beschreibt gegebenenfalls ob daten maskiert werden sollen */
        IntEn = 0x10, /* Diverse Intererupt einstellungen*/
        AccAMThresh = 0x11, /* Accelerometer Any Motion Threshold, lowest value dependant of g*/
        AccIntSettings = 0x12, /* acc interupt settings*/
        AccHGDuration = 0x13, /* High G dauer*/
        AccHGThresh = 0x14,
        AccNMThresh = 0x15,
        AccNMSet = 0x16,
        GyrIntSettings = 0x17,
        GyrHRXSet = 0x18, /* High rate x axis*/
        GyrDurX = 0x19, /*High rate duration in (1+ value)*2.5 ms*/
        GyrHRYSet = 0x1A,
        GyrDurY = 0x1B,
        GyrHRZSet = 0x1C,
        GyrDurZ = 0x1D,
        GYRAMThresh = 0x1E, /* Gyr Any Motion Threshold */
        GyrAMSet = 0x1F, /* Gyr Any Motion Threshhold */
    };

    enum BNO055SETTINGS
    {
        AccSettings = 0b00011111, /* Wenn NDOF, dann unn???tig da automatisch*/
        GyrSettings = 0b00000000,
        MagSettings = 0b00011111,
        UnitSettings = 0b00000000, /*m/s^2 , mT, DPS, Degrees, Quat units, ???C, outputformat Windows. Change first bit to 1 for android definition */

    };
public:
	explicit  BNO055_IMU(float updateFreq);

	void init() override;
    void update() override;
    std::unique_ptr<Data> getData() override;
    SensorType getSensorType() override;
    int getI2CAddr() override;

	void reset();
	int calibrate();
	void config();
    bool is_online();
};
	


#endif //IMU_TEST_BNO055_H