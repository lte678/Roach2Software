#include "Sensor_Manager.h"

Sensor_Manager::Sensor_Manager()
{
    std::cout << "[Sensor Manager] Initializing" << std::endl;
    logging_rate = 20.0f;
	number_sensors = 0;

	// Create and open logging file (check if already exist and increase name)
	bool file_exists = true;
	std::string filename;
	int counter_f = 0;
	while (file_exists) {
		filename = this->filename_logging + std::to_string(counter_f) + ".csv";
        file_exists = (access(filename.c_str(), F_OK ) != -1);
		counter_f++;
	}
	logging_stream = open(filename.c_str(), O_WRONLY | O_CREAT);
    std::cout << "[Sensor Manager] Using log file " << filename << std::endl;
}

/** 
 * @brief Sensor main loop running in separate run, this function should be called by std::thread
*/
void Sensor_Manager::run()
{
    std::cout << "[Sensor Manager] Started logging thread" << std::endl;

	// Init sensor loop
	unsigned int delay = (unsigned int)((1.0 / (double)logging_rate) * 1e6); // delay for usleep in us
	stop_bit = false;
	// Sensor loop
	while (!stop_bit) {
		usleep(delay); // Loop interval sleep

        unsigned int time = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now().time_since_epoch())).count();
        sensor_lock.lock();
		for (Sensor *sensor : sensors) {
			std::unique_ptr<Data> data = sensor->getData();
			if (data) {
                // Store to csv file for logging
                std::string logText;
                logText += "TIME:";
                logText += std::to_string(time);
                logText += ";";
                logText += data->serializeLogging();
                logText += "\n";
                const char* cString = logText.c_str();
                write(logging_stream, cString, strlen(cString));
			}
		}
        sensor_lock.unlock();
		fsync(logging_stream);
	}

	close(logging_stream);
}

/**
 * @brief Stops the sensor manager thread. This sets the stop which terminates the thread main loop.
*/
void Sensor_Manager::stop()
{
	this->stop_bit.store(true);
}

bool Sensor_Manager::getData(std::unique_ptr<Data>& data_ptr, SensorType sensor_id)
{   sensor_lock.lock();
	// Load sensor value
	for(Sensor *sensor : sensors) {
	    if(sensor->getSensorType() == sensor_id) {
            data_ptr = sensor->getData();
	    }
	}
    sensor_lock.unlock();

	// Return true if not null
    return data_ptr != nullptr;
}

Sensor* Sensor_Manager::getSensorHandle(SensorType sensor_id) {
    sensor_lock.lock();
    for(Sensor *sensor : sensors) {
        if(sensor->getSensorType() == sensor_id) {
            sensor_lock.unlock();
            return sensor;
        }
    }
    sensor_lock.unlock();

    return nullptr;
}

void Sensor_Manager::attachSensor(Sensor* sensor) {
    if(sensor == nullptr) {
        return;
    }

    sensor_lock.lock();

    // Check if duplicate
    for(Sensor *existing_sensor : sensors) {
        if(existing_sensor->getSensorType() == sensor->getSensorType()) {
            std::cout << "[Sensor Manager] Duplicate sensor! Skipping." << std::endl;
            sensor_lock.unlock();
            return;
        }
    }

    sensor->init();
    sensorThreads.emplace_back(&Sensor::run, sensor);
    number_sensors++;
    sensors.push_back(sensor);

    sensor_lock.unlock();
}