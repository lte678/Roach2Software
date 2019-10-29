/**
* @file imu.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "imu_neu.h"

/*#include "Roach2_Sensor_Sensor.h"*/

BNO055_IMU::BNO055_IMU()
{
	deviceHandle = this->i2cConnect(BNO055_DEVICE_ID); // Get file/I2C handle
	std::cout << "BNO055 ctor device handle: " << this->deviceHandle << " " << deviceHandle << std::endl;

	if (this->deviceHandle == -1)
	{
		std::cout << "IMU: Connection failed!" << std::endl;
	}
	if (is_online()) //checks if online
	{
		std::cout << "IMU: online" << std::endl;
	}
	else
	{
		std::cout << "IMU: not online! Performing reset.." << std::endl;
		this->reset();
	}
}

bool BNO055_IMU::is_online()
{
	/* Checks if BNO055 is online by reading the chip id register */
	int ret = (this->read8(CHIP_ID_ADDR));
	std::cout << "Is online: Should be 160 -> " << ret << std::endl;
	if(ret == 0xA0) {
		return true;
	}
	else {
		return false;
	}
}
void BNO055_IMU::init() //performs all settings possible
{
	this->config();
	//this->calibrate();
}
int BNO055_IMU::getSensorType()
{
	return 0; //test
}
void BNO055_IMU::update()
{
	this->measurement = ((this->read8(ACCEL_DATA_X_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_X_LSB_ADDR)); /*Get Acceleration. 100LSB = 1m/s^2*/

	this->acc[0] = this->changeSign(measurement)*0.01;

	this->measurement = ((this->read8(ACCEL_DATA_Y_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_Y_LSB_ADDR));

	this->acc[1] = this->changeSign(measurement)*0.01;

	this->measurement = ((this->read8(ACCEL_DATA_Z_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_Z_LSB_ADDR));

	this->acc[2] = this->changeSign(measurement)*0.01;
	/*---------------------------------------------------------------------------------------------------------*/
	this->measurement = ((this->read8(MAG_DATA_X_MSB_ADDR) << 8) + this->read8(MAG_DATA_X_LSB_ADDR)); /*Magnetometerdata. 16LSB = 1microT */

	this->mag[0] = this->changeSign(measurement)*0.0625;
	
	this->measurement = ((this->read8(MAG_DATA_Y_MSB_ADDR) << 8) + this->read8(MAG_DATA_Y_LSB_ADDR));
	
	this->mag[1] = this->changeSign(measurement)*0.0625;
	
	this->measurement = ((this->read8(MAG_DATA_Y_MSB_ADDR) << 8) + this->read8(MAG_DATA_Y_LSB_ADDR));
	
	this->mag[2] = this->changeSign(measurement)*0.0625;
	/*------------------------------------------------------------------------------------------------------------*/
	this->measurement = ((this->read8(GYRO_DATA_X_MSB_ADDR) << 8) + this->read8(GYRO_DATA_X_LSB_ADDR)); /*Gyroscopedata. 16LSB = 1DPS */

	this->gyr[0] = this->changeSign(measurement)*0.0625;
	
	this->measurement = ((this->read8(GYRO_DATA_Y_MSB_ADDR) << 8) + this->read8(GYRO_DATA_Y_LSB_ADDR)); 

	this->gyr[1] = this->changeSign(measurement)*0.0625;

	this->measurement = ((this->read8(GYRO_DATA_Z_MSB_ADDR) << 8) + this->read8(GYRO_DATA_Z_LSB_ADDR));

	this->gyr[2] = this->changeSign(measurement)*0.0625;
	/*------------------------------------------------------------------------------------------------------------*/
	this->measurement = ((this->read8(QUATERNION_DATA_W_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_W_LSB_ADDR)); /*Quaterniondata. 2^(14)LSB = 1Quaternion */

	this->quat[0] = this->changeSign(measurement)*0.00006103515;
	
	this->measurement = ((this->read8(QUATERNION_DATA_X_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_X_LSB_ADDR)); 

	this->quat[1] = this->changeSign(measurement)*0.00006103515;

	this->measurement = ((this->read8(QUATERNION_DATA_Y_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_Y_LSB_ADDR));

	this->quat[2] = this->changeSign(measurement)*0.00006103515;

	this->measurement = ((this->read8(QUATERNION_DATA_Z_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_Z_LSB_ADDR)); 

	this->quat[3] = this->changeSign(measurement)*0.00006103515;
		/*------------------------------------------------------------------------------------------------------------*/
	this->measurement = ((this->read8(EULER_R_MSB_ADDR) << 8) + this->read8(EULER_R_MSB_ADDR)); /*Euler. 16LSB = 1DPS */

	this->rot[0] = this->changeSign(measurement)*0.0625;

	this->measurement = ((this->read8(EULER_P_MSB_ADDR) << 8) + this->read8(EULER_P_MSB_ADDR));

	this->rot[1] = this->changeSign(measurement)*0.0625;

	this->measurement = ((this->read8(EULER_H_MSB_ADDR) << 8) + this->read8(EULER_H_MSB_ADDR));

	this->rot[2] = this->changeSign(measurement)*0.0625;
	/*------------------------------------------------------------------------------------------------------------*/
	this->measurement = this->read8(TEMP_ADDR);
	if ((this->measurement >> 7) == 1)
	{
		int change = 0b10000000;
		measurement = change | measurement;
		measurement = binaryToDecimal(measurement);
		measurement = measurement * (-1);
	}
	this->temp = this->measurement*1.0;
}

int BNO055_IMU::changeSign(int measurement)
{
	if ((measurement >> 15) != 0)
	{
		int signchange = 0b1000000000000000;
		measurement = measurement | signchange;
		measurement = binaryToDecimal(measurement);
			measurement = measurement * -1;
	}
	else
	{
		measurement = binaryToDecimal(measurement);
	}
}

Data* BNO055_IMU::getData()
{
	Data* data_ptr = new Data();
	data_ptr->addValue("ACCELERATION_X", this->acc[0]);
	data_ptr->addValue("ACCELERATION_Y", this->acc[1]);
	data_ptr->addValue("ACCELERATION_Z", this->acc[2]);
	data_ptr->addValue("NAG_X", this->mag[0]);
	data_ptr->addValue("NAG_Y", this->mag[1]);
	data_ptr->addValue("NAG_Z", this->mag[2]);
	data_ptr->addValue("GYR_X", this->gyr[0]);
	data_ptr->addValue("GYR_Y", this->gyr[1]);
	data_ptr->addValue("GYR_Z", this->gyr[2]);
	data_ptr->addValue("QUAT_W", this->quat[0]);
	data_ptr->addValue("QUAT_X", this->quat[1]);
	data_ptr->addValue("QUAT_Y", this->quat[2]);
	data_ptr->addValue("QUAT_Z", this->quat[3]);
	data_ptr->addValue("EULER_R", this->rot[0]);
	data_ptr->addValue("EULER_P", this->rot[1]);
	data_ptr->addValue("EULER_H", this->rot[2]);
	data_ptr->addValue("imu_temp", this->temp);
	return data_ptr;
}

int BNO055_IMU::getI2CAddr()
{
	return BNO055_DEVICE_ID;
}



void BNO055_IMU::reset()
{
	/* Resets BNO055 device */
	if (this->is_online())
		this->write8(SYS_TRIGGER_ADDR, 0x00);
}

int BNO055_IMU::calibrate() 
{
	/* Calibrates BNO055 with help of User Action*/
	std::cout << "BNO055: Calibration started, timeout " << CALIBRATION_TIMEOUT_SECONDS << " seconds.\n" << "BNO055: Please move and rotate device slowly with several stops that last a few seconds." << std::endl;
	// Take time and current calibration status
	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();
	long elapsed_seconds = 0;
	int old_calibration_status = this->read8(CALIB_STAT_ADDR);
	int calibration_status = old_calibration_status;
	// Check for calibration completion
	while ((calibration_status != 0b11111111) && (elapsed_seconds < CALIBRATION_TIMEOUT_SECONDS)) {
		if (calibration_status > old_calibration_status) {
			std::cout << "BNO055: Still calibrating, continue motion..." << std::endl;
			old_calibration_status = calibration_status;
		}
		elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
		calibration_status = this->read8(CALIB_STAT_ADDR);
	}
	// Return calibration status
	if (calibration_status == 0b11111111) {
		std::cout << "BNO055: Calibration successfully finished!" << std::endl;
		return EXIT_SUCCESS;
	}
	else {
		std::cout << "BNO055: Impartial or failed calibration. Calibration status " << std::bitset<8>(this->read8(CALIB_STAT_ADDR)) << "." << std::endl;
		return EXIT_FAILURE;
	}

}

void BNO055_IMU::config() 
{
	int mode = NDOF_CONF;  /*Operationmode hier ändern!*/
		write8(OPR_MODE_ADDR, mode); //alternativ ACC_MAG_GYRO_CONF
	if (mode != NDOF_CONF)
	{
		write8(PAGE_ID_ADDR, 0x01); /* Ändert logische Registerseite*/
		write8(AccConf, AccSettings); /* Stellt Beschleunigungssensor ein*/
		write8(MagConf, MagSettings); /* Stellt Magnetometer ein*/
		write8(PAGE_ID_ADDR, 0x00); /* Ändert logische Registerseite zurück */
	}
	write8(UNIT_SEL_ADDR, UnitSettings);
}




/*IMU(bool force_calibration = true, BNO055_OPERATION_MODES_t mode = NDOF_CONF) {
	// Initialize BNO055 device
	std::cout << "BNO055: Started Initialization" << std::endl;
	if (this->initialize(force_calibration, mode)) {
		std::cout << "BNO055: Set Operating mode to " << std::bitset<8>(this->read8(OPR_MODE_ADDR)) << std::endl;
		std::cout << "BNO055: Initialization finished!" << std::endl;
	}
	else {
		std::cout << "BNO055: Initialization failed! Could not connect to BNO055 @" << BNO055_DEVICE_ID << ". Check Wiring and Device Id!" << std::endl;
	}
} */
/*int initialize(bool force_calibration, BNO055_OPERATION_MODES_t mode) {
	/* Initialize BNO055 and start calibration if wanted
	this->_device_handle = wiringPiI2CSetup(BNO055_DEVICE_ID);
	if (this->_device_handle != -1) {
		// Reset Device
		if (force_calibration) this->reset();
		// Set Operating Mode
		this->write8(OPR_MODE_ADDR, mode);
		// Calibrate
		if (force_calibration) this->calibrate();
		// Update values
		this->update();
		return EXIT_SUCCESS;
	}
	else {
		return EXIT_FAILURE;
	}
} */