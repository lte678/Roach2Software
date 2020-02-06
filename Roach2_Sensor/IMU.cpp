/**
* @file imu.cpp
* 22-08-2019
* @brief the overview of the class
*
* @copyright KSat Stuttgart e.V. Roach2 software team
*/
#include "IMU.h"

/*#include "Roach2_Sensor_Sensor.h"*/

BNO055_IMU::BNO055_IMU(float updateFreq) : Sensor(updateFreq)
{
    std::cout << "[Sensor|IMU] Initializing" << std::endl;
	deviceHandle = this->i2cConnect(BNO055_DEVICE_ID); // Get file/I2C handle
	std::cout << "[Sensor|IMU] ctor device handle: " << deviceHandle << std::endl;

	if (deviceHandle == -1)
	{
		std::cout << "[Sensor|IMU] Connection failed!" << std::endl;
	}
	if (is_online()) //checks if online
	{
		std::cout << "[Sensor|IMU] Online" << std::endl;
	}
	else
	{
		std::cout << "[Sensor|IMU] Offline!" << std::endl;
	}
}

bool BNO055_IMU::is_online()
{
	/* Checks if BNO055 is online by reading the chip id register */
	int ret = (this->read8(CHIP_ID_ADDR));
    // Online if return value matches
    return ret == 0xA0;
}
void BNO055_IMU::init() //performs all settings possible
{
	this->config();
	//this->calibrate();
}
SensorType BNO055_IMU::getSensorType()
{
	return SensorType::IMU; //test
}
void BNO055_IMU::update()
{
    int16_t measurement;
	measurement = ((this->read8(ACCEL_DATA_X_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_X_LSB_ADDR)); /*Get Acceleration. 100LSB = 1m/s^2*/
	acc[0] = (double)measurement / 100.0;

	measurement = ((this->read8(ACCEL_DATA_Y_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_Y_LSB_ADDR));
	acc[1] = (double)measurement / 100.0;

	measurement = ((this->read8(ACCEL_DATA_Z_MSB_ADDR) << 8) + this->read8(ACCEL_DATA_Z_LSB_ADDR));
	acc[2] = (double)measurement / 100.0;
	/*---------------------------------------------------------------------------------------------------------*/
	measurement = ((this->read8(MAG_DATA_X_MSB_ADDR) << 8) + this->read8(MAG_DATA_X_LSB_ADDR)); /*Magnetometerdata. 16LSB = 1microT */
	mag[0] = (double)measurement / 16.0;
	
	measurement = ((this->read8(MAG_DATA_Y_MSB_ADDR) << 8) + this->read8(MAG_DATA_Y_LSB_ADDR));
	mag[1] =(double)measurement / 16.0;
	
	measurement = ((this->read8(MAG_DATA_Z_MSB_ADDR) << 8) + this->read8(MAG_DATA_Z_LSB_ADDR));
	mag[2] = (double)measurement / 16.0;
	/*------------------------------------------------------------------------------------------------------------*/
	measurement = ((this->read8(GYRO_DATA_X_MSB_ADDR) << 8) + this->read8(GYRO_DATA_X_LSB_ADDR)); /*Gyroscopedata. 16LSB = 1DPS */
	gyr[0] = (double)measurement / 16.0;
	
	measurement = ((this->read8(GYRO_DATA_Y_MSB_ADDR) << 8) + this->read8(GYRO_DATA_Y_LSB_ADDR));
	gyr[1] = (double)measurement / 16.0;

	measurement = ((this->read8(GYRO_DATA_Z_MSB_ADDR) << 8) + this->read8(GYRO_DATA_Z_LSB_ADDR));
	gyr[2] = (double)measurement / 16.0;
	/*------------------------------------------------------------------------------------------------------------*/
	uint16_t umeasurement;
	umeasurement = ((this->read8(QUATERNION_DATA_W_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_W_LSB_ADDR)); /*Quaterniondata. 2^(14)LSB = 1Quaternion */
	quat[0] = (double)umeasurement / (double)(1 << 14);
	
	umeasurement = ((this->read8(QUATERNION_DATA_X_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_X_LSB_ADDR));
	quat[1] = (double)umeasurement / (double)(1 << 14);

	umeasurement = ((this->read8(QUATERNION_DATA_Y_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_Y_LSB_ADDR));
	quat[2] = (double)umeasurement / (double)(1 << 14);

	umeasurement = ((this->read8(QUATERNION_DATA_Z_MSB_ADDR) << 8) + this->read8(QUATERNION_DATA_Z_LSB_ADDR));
	quat[3] = (double)umeasurement / (double)(1 << 14);
		/*------------------------------------------------------------------------------------------------------------*/
	measurement = ((this->read8(EULER_R_MSB_ADDR) << 8) + this->read8(EULER_R_MSB_ADDR)); /*Euler. 16LSB = 1DPS */
	rot[0] = (double)measurement / 16.0;

	measurement = ((this->read8(EULER_P_MSB_ADDR) << 8) + this->read8(EULER_P_MSB_ADDR));
	rot[1] = (double)measurement / 16.0;

	measurement = ((this->read8(EULER_H_MSB_ADDR) << 8) + this->read8(EULER_H_MSB_ADDR));
	rot[2] = (double)measurement / 16.0;
	/*------------------------------------------------------------------------------------------------------------*/
	int8_t tmeasurement = read8(TEMP_ADDR);
	temp = (double)tmeasurement;
}

std::unique_ptr<Data> BNO055_IMU::getData()
{
	std::unique_ptr<Data> data_ptr(new Data());
	data_ptr->setId((int)SensorType::IMU);
    data_ptr->addValue("TEMP", this->temp);
	data_ptr->addValue("ACC_X", this->acc[0]);
	data_ptr->addValue("ACC_Y", this->acc[1]);
	data_ptr->addValue("ACC_Z", this->acc[2]);
	data_ptr->addValue("MAG_X", this->mag[0]);
	data_ptr->addValue("MAG_Y", this->mag[1]);
	data_ptr->addValue("MAG_Z", this->mag[2]);
	data_ptr->addValue("GYR_X", this->gyr[0]);
	data_ptr->addValue("GYR_Y", this->gyr[1]);
	data_ptr->addValue("GYR_Z", this->gyr[2]);

	int cali_stat = this->read8(CALIB_STAT_ADDR);

	data_ptr->addValue("Cali_stat", (double)cali_stat);
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
	std::cout << "[Sensor|IMU] Calibration started, timeout " << CALIBRATION_TIMEOUT_SECONDS << " seconds." << std::endl;
	std::cout << "[Sensor|IMU] Please move and rotate device slowly with several stops that last a few seconds." << std::endl;
	// Take time and current calibration status
	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();
	long elapsed_seconds = 0;
	int old_calibration_status = this->read8(CALIB_STAT_ADDR);
	int calibration_status = old_calibration_status;
	// Check for calibration completion
	while ((calibration_status != 0b11111111) && (elapsed_seconds < CALIBRATION_TIMEOUT_SECONDS)) {
		if (calibration_status > old_calibration_status) {
			std::cout << "[Sensor|IMU] Still calibrating, continue motion..." << std::endl;
			old_calibration_status = calibration_status;
		}
		elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
		calibration_status = this->read8(CALIB_STAT_ADDR);
	}
	// Return calibration status
	if (calibration_status == 0b11111111) {
		std::cout << "[Sensor|IMU] Calibration successfully finished!" << std::endl;
		return EXIT_SUCCESS;
	}
	else {
		std::cout << "[Sensor|IMU] Impartial or failed calibration. Calibration status " << std::bitset<8>(this->read8(CALIB_STAT_ADDR)) << "." << std::endl;
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