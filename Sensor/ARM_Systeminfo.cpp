#include "ARM_Systeminfo.h"
#include <fcntl.h>

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


	// See: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	//Add other values in next statement to avoid int overflow on right hand side...
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	totalPhysMem = memInfo.totalram;
	//Multiply in next statement to avoid int overflow on right hand side...
	totalPhysMem *= memInfo.mem_unit; // Physical total amount RAM in Bytes
	physMemUsed = memInfo.totalram - memInfo.freeram;
	//Multiply in next statement to avoid int overflow on right hand side...
	physMemUsed *= memInfo.mem_unit; // Physical amount of RAM used in Bytes

	int FileHandler;
	char FileBuffer[1024];
	float load;

	// Source https://www.raspberrypi.org/forums/viewtopic.php?t=64835 https://linuxwiki.de/proc/loadavg
	// Interpretation: as we have 4 cores => 4 is full load, any value below is meaning that the system is not
	// fully used. Values above 4 would indicate overload (not possible for CPU to execute all tasks in the defined time)
	FileHandler = open("/proc/loadavg", O_RDONLY);
	read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
	sscanf(FileBuffer, "%f", &load);
	close(FileHandler);
	percent = load * 100.0f;

	// CPU temperature, value is in /sys/devices/virtual/thermal/thermal_zone0/temp => Nano Pi Neo Air has one temperature sensor there (thermal_zone0)
	int fd;
	char buffer[6];
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
