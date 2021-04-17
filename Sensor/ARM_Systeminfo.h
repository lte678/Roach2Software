#ifndef ARM_SYSTEMINFO_HEADER_FILE
#define ARM_SYSTEMINFO_HEADER_FILE

#include "Sensor.h"
#include <fcntl.h>
#include "../DataStore/data.h"
#include "../DataStore/Data_simple.h"
#include <unistd.h>
#include <fstream>
#include <string>

/* @file ARM_Systeminfo.h
 * @date 29.08.2019
 * @brief ARM system status information readout class
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */
class ARM_Systeminfo :
	public Sensor
{
private:
	Data* data_obj;

    unsigned long totalPhysMem;
    unsigned long physMemUsed;
    float percent;
    float temp;

    // Variables used for cpu usage calculation:
    // Yes I know jiffies sound stupid; but that's what they (1/100th of a second) are called :)
    unsigned long last_jiffies;
    unsigned long last_jiffies_worked;
public:
    explicit ARM_Systeminfo(float updateFreq);
	void init();
	void update();
    std::unique_ptr<Data> getData();
	int getI2CAddr();
	/**
	* @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
	* @return int sensor identifier
	*/
    SensorType getSensorType();
};

#endif