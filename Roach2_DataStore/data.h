/* @file data.h
 * @date 05.06.2019
 * @brief Definition of the Data structure class used in the Roach2 experiment for
 * communication and saving.
 *
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef DATA_H
#define DATA_H

#include <vector>
#include "Data_super.h"
#include <string>
#include <iostream>
#include "serialization.h"
#include <string.h>

enum SENSOR_TYPES {
	TEMP_SENSOR = 0,
	IMU = 1,
	LASERDIST_0 = 2,
	LASERDIST_1 = 3,
	LASERDIST_2 = 4,
	LASERDIST_3 = 5,
	LASERDIST_4 = 6,
	LASERDIST_5 = 7,
	ADC = 8,
	ROT_ENC = 9,
	SYS_INFO = 10
};

enum IMU_FIELD_TYPES {
	ACCELERATION_X = 0,
	ACCELERATION_Y = 1,
	ACCELERATION_Z = 2,
	MAG_X = 3,
	MAG_Y = 4,
	MAG_Z = 5,
	GYR_X = 6,
	GYR_Y = 7,
	GYR_Z = 8,
	imu_temp = 9
};

/**
 * @brief Converts string imu_field_name to integer code
 * @param imu_field_name string field name
 * @return int according to enum IMU_FIELD_TYPES
*/
static IMU_FIELD_TYPES convert_imu_field(char* imu_field_name) {
	if (strcmp(imu_field_name, "ACCELERATION_X") == 0) {
		return ACCELERATION_X;
	}
	else if (strcmp(imu_field_name, "ACCELERATION_Y")) {
		return ACCELERATION_Y;
	}
	else if (strcmp(imu_field_name, "ACCELERATION_Z")) {
		return ACCELERATION_Z;
	}
	else if (strcmp(imu_field_name, "MAG_X")) {
		return MAG_X;
	}
	else if (strcmp(imu_field_name, "MAG_Y")) {
		return MAG_Y;
	}
	else if (strcmp(imu_field_name, "MAG_Z")) {
		return MAG_Z;
	}
	else if (strcmp(imu_field_name, "GYR_X")) {
		return GYR_X;
	}
	else if (strcmp(imu_field_name, "GYR_Y")) {
		return GYR_Y;
	}
	else if (strcmp(imu_field_name, "GYR_Z")) {
		return GYR_Z;
	}
	else if (strcmp(imu_field_name, "imu_temp")) {
		return imu_temp;
	}

}

class Data : public Data_super {
    /*
     * @brief Data Structure used for communication and data saving. Every
     * instance can contain one value per column
     * */

    private:
        int id;                         // id of Data object
        unsigned long time;                     // timestamp
        std::vector<std::string> columnNames;   // names of data columns
        std::vector<double> values;             // data to hold

    public:
        // constructors
        Data();
        Data(int id,
             unsigned long time,
             std::vector<std::string> columnNames,
             std::vector<double> values);

        // set & get
        void setId(int id);
        int getId();

        void setTime(const unsigned long time);
        long getTime();

        void setColumnNames(const std::vector<std::string> columnNames);
        std::vector<std::string> getColumnNames();
		void amendColumnName(std::string name);

        void setValues(const std::vector<double> values);
        std::vector<double> getValues();
		void amendValue(double value);

		void addValue(std::string type, double value);

        // serialize & deserialize
        std::string serialize();
        void deserialize(std::string * archive);

        // self check if valid data
        bool is_valid();

        // pretty print
        void print();

		uint64_t* convert_to_serial();
		int convert_to_serial_array_length();

};


#endif
