#ifndef ARM_SYSTEMINFO_HEADER_FILE
#define ARM_SYSTEMINFO_HEADER_FILE

#include "Roach2_Sensor_Sensor.h"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"

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
public:
	void init();
	void update();
	Data* getData();
	int getI2CAddr();
	/**
	* @brief Returns the sensor identifier number (see SENSOR_TYPES in data.h from datastore project)
	* @return int sensor identifier
	*/
	int getSensorType();
};

#endif