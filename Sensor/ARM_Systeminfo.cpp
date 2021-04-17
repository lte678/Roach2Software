#include "ARM_Systeminfo.h"

ARM_Systeminfo::ARM_Systeminfo(float updateFreq) : Sensor(updateFreq) {
    // Log for consistencies sake. Not really necessary.
    std::cout << "[Sensor|ARM] Object initialized" << std::endl;
}

void ARM_Systeminfo::init()
{
	// Not required here as we need to access
}

/**
 * @brief Reads the sensor out
*/
void ARM_Systeminfo::update()
{
    int fd;
    char buffer[4096];

    // https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c
    std::ifstream file("/proc/meminfo");
    std::string line;
    while(std::getline(file, line)) {
        char key[64];
        unsigned long value;
        sscanf(line.c_str(), "%s %lu", key, &value);
        if(std::string(key) == "MemTotal:") {
            totalPhysMem = value;
        } else if(std::string(key) == "MemAvailable:") {
            physMemUsed = totalPhysMem - value;
        }
    }

	// https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c
    fd = open("/proc/stat", O_RDONLY);
	read(fd, buffer, sizeof(buffer) - 1);

	unsigned long int n[10];
	sscanf(buffer, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6], &n[7], &n[8], &n[9]);
    unsigned int jiffies = 0;
	for(int i = 0; i < 10; i++) {
	    jiffies += n[i];
	}
	unsigned long jiffies_worked = jiffies - n[3];
	percent = (float)(jiffies_worked - last_jiffies_worked) / (float)(jiffies - last_jiffies) * 100.0f;
	last_jiffies_worked = jiffies_worked;
	last_jiffies = jiffies;
	close(fd);

	// CPU temperature, value is in /sys/devices/virtual/thermal/thermal_zone0/temp => Nano Pi Neo Air has one temperature sensor there (thermal_zone0)
	fd = open("/sys/devices/virtual/thermal/thermal_zone0/temp", O_RDONLY);
	read(fd, buffer, sizeof(buffer) - 1);
	sscanf(buffer, "%f", &temp);
	close(fd);
	temp = temp / 1000.0f;
}

/**
 * @brief Returns the sensor output
*/
std::unique_ptr<Data> ARM_Systeminfo::getData()
{
	// Create new data object
    std::unique_ptr<Data> data_obj(new Data());

	data_obj->setId((int)SensorType::SYS_INFO);
	data_obj->addValue("MEM_TOTAL", totalPhysMem);
	data_obj->addValue("MEM_USED", physMemUsed);
	data_obj->addValue("CPU_USAGE", percent);
	data_obj->addValue("TEMP", temp);

	return data_obj;
}

/**
 * @brief Note, this sensor uses no I2C interface!
*/
int ARM_Systeminfo::getI2CAddr()
{
	return 0;
}

SensorType ARM_Systeminfo::getSensorType()
{
	return SensorType::SYS_INFO;
}
