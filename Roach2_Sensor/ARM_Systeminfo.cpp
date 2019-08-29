#include "ARM_Systeminfo.h"

void ARM_Systeminfo::init()
{
	// Not required here as we need to access
}

/**
 * @brief Reads the sensor out
*/
void ARM_Systeminfo::update()
{
	// Create new data object
	this->data_obj = new Data();

	// See: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	//Add other values in next statement to avoid int overflow on right hand side...
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	long long totalPhysMem = memInfo.totalram;
	//Multiply in next statement to avoid int overflow on right hand side...
	totalPhysMem *= memInfo.mem_unit; // Physical total amount RAM in Bytes
	long long physMemUsed = memInfo.totalram - memInfo.freeram;
	//Multiply in next statement to avoid int overflow on right hand side...
	physMemUsed *= memInfo.mem_unit; // Physical amount of RAM used in Bytes

	// RAM usage
	this->data_obj->addValue("MEM_TOTAL", totalPhysMem);
	this->data_obj->addValue("MEM_USED", physMemUsed);

	// CPU usage
	unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
	FILE* file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
		&lastTotalSys, &lastTotalIdle);
	fclose(file);
	double percent;
	FILE* file;
	unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
		&totalSys, &totalIdle);
	fclose(file);

	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
		totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else {
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
			(totalSys - lastTotalSys);
		percent = total;
		total += (totalIdle - lastTotalIdle);
		percent /= total;
		percent *= 100;
	}
	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;

	this->data_obj->addValue("CPU_USAGE", percent);
}

/**
 * @brief Returns the sensor output
*/
Data* ARM_Systeminfo::getData()
{
	return this->data_obj;
}

/**
 * @brief Note, this sensor uses no I2C interface!
*/
int ARM_Systeminfo::getI2CAddr()
{
	return 0;
}
