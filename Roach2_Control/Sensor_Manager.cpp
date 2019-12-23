#include "Sensor_Manager.h"

Sensor_Manager::Sensor_Manager(bool obc, bool rcu, EthernetClient* client, EthernetServer* server)
{
	this->update_rate = 1;
	this->sensor_values_loaded = 0; // Reset counter

	// Create and open logging file (check if already exist and increase name)
	bool file_new = false;
	std::string filename;
	int counter_f = 0;
	while (!file_new) {
		filename = this->filename_logging + std::to_string(counter) + ".csv";
		std::ifstream f(filename.c_str());
		file_new = f.good();
		counter++;
	}
	this->logging_stream = new std::ofstream(filename.c_str());

	// Create all sensors
	if (obc) {
		/**
		 * OBC sensor list:
		 * -> 1x BNO055
		 * -> 1x ARM systeminfo
		 * -> 1x LM75B
		 * -> 1x ADC
		*/
		ARM_Systeminfo* info = new ARM_Systeminfo();
		info->init();
		
		TEMP_LM75B *temp = new TEMP_LM75B();
		temp->init();

		BNO055_IMU *imu = new BNO055_IMU();
		imu->init();

		ADC_MCP3428* adc = new ADC_MCP3428();
		adc->init();

		OBC_Systemstatus* obc_info = new OBC_Systemstatus(client, server);
		obc_info->init();

		this->sensors = {info, temp, imu, adc, obc_info};
		this->number_sensors = 5;
	}
	else if (rcu) {
		/**
		 * RCU sensor list:
		 * 
		*/
		ARM_Systeminfo* info = new ARM_Systeminfo();
		info->init();

		TEMP_LM75B* temp = new TEMP_LM75B();
		temp->init();

		ADC_MCP3428* adc = new ADC_MCP3428();
		adc->init();

		BNO055_IMU* imu = new BNO055_IMU();
		imu->init();

		ROT_AS5601* rot = new ROT_AS5601();
		rot->init();

		RCU_Systemstatus* rcu_info = new RCU_Systemstatus(client, server);

		this->sensors = { info, temp, imu, rot, adc, rcu_info};
		this->number_sensors = 6;
	}
}

/** 
 * @brief Sensor main loop running in separate run, this function should be called by std::thread
*/
void Sensor_Manager::run(void)
{
	// Init sensor loop
	double delay = ceil((1 / (double)this->update_rate)*1e6); // delay for usleep in us
	this->stop_bit.store(false); // Disable stop variable

	// Sensor loop
	while (!(this->stop_bit.load())) {
		// Delay
		usleep(delay);
		
		// Critical area, access buffer
		this->lock_data_access.lock();

		// Update all sensors

		unsigned int time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();

		for (int i = 0; i < this->number_sensors; i++) {
			this->sensors[i]->update();
			Data* data = this->sensors[i]->getData();
			this->loaded_data.push_back(data);
			this->sensor_values_loaded++;
			
			// Store to csv file for logging	
			std::string text = data->serializeLogging();
			*(this->logging_stream) << "TIME:" << time << ":";
			*(this->logging_stream) << text << std::endl;
		}
		// Clear sensor data log if more than 25 elements in memory
		if (this->loaded_data.size() > 25) {
			this->loaded_data.clear();
		}			

		// Leave critical area
		this->lock_data_access.unlock();
	}

	this->logging_stream->close();
}

/**
 * @brief Stops the sensor manager thread. This sets the stop which terminates the thread main loop.
*/
void Sensor_Manager::stop(void)
{
	this->stop_bit.store(true);
}

bool Sensor_Manager::getData(Data** data_ptr, SENSOR_TYPES sensor_id)
{
	// Need to get access to lock
	this->lock_data_access.lock();

	// Load sensor value
	for (int i = 0; i < this->loaded_data.size(); i++) {
		if (this->loaded_data[i]->getId() == (int)sensor_id) {
			*data_ptr = this->loaded_data[i];
			this->lock_data_access.unlock();
		}
	}

	// Unlock
	this->lock_data_access.unlock();

	if (data_ptr != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

/**
 * @brief Returns the latest element of the loaded data from all active sensors, if no data is available, return false.
 *		  Function is blocking on call!
 * @param available_Data: Pointer which will hold the address of the last data object if available (note the address to the pointer is passed)
 *					This is required as pointer are passed as copy of the pointer value, so any change to the pointer itself is not valid
 *					outside of the function.
 * @return true if data found and returned, false otherwise
*/
bool Sensor_Manager::getData(std::vector<Data*> *available_Data)
{
	bool res = false;

	// Need to get access to lock
	this->lock_data_access.lock();

	// Return last data object
	while (this->loaded_data.size() > 0) {
		res = true;
		available_Data->push_back(this->loaded_data.back());
		this->loaded_data.pop_back();
	}
	
	// Unlock
	this->lock_data_access.unlock();

	return res;
}

/**
 * @brief Sets the update rate (in Hz)
 * @param update_rate: Update rate in Hz, defines how often the sensor manager fetch sensor values
*/
void Sensor_Manager::setUpdateRate(int update_rate)
{
	this->update_rate = update_rate;
}

/**
 * @brief Returns the number of sensor values loaded upto the current moment
 *		  Function is blocking on call!
 * @return int number of loaded sensor values
*/
int Sensor_Manager::getStatNumberSensorValuesLoaded(void)
{
	int res = 0;

	// Acquire lock
	this->lock_data_access.lock();
	
	res = this->sensor_values_loaded;

	// Release lock
	this->lock_data_access.unlock();

	return res;
}
