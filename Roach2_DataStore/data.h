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
#include <string>
#include <cstdio>
#include "serialization.h"

enum DATA_FRAME_IDS {
	/*
	 * @brief Enumeration used for identification of Data objects.
	 * */
	COMMAND = 0x01,
	STATUS = 0x02,
	SENSOR_IMU = 0x03,
	SENSOR_ROT_ENCODER = 0x04,
	SENSOR_TEMP = 0x05,
	SENSOR_ADC = 0x06,
	SENSOR_LASER1 = 0x07,
	SENSOR_LASER2 = 0x08,
	SENSOR_LASER3 = 0x09,
	SENSOR_LASER4 = 0x0A,
	SENSOR_LASER5 = 0x0B,
	SENSOR_LASER6 = 0x0C
};


class Data {
	/*
	 * @brief Data Structure used for communication and data saving. Every
	 * instance can contain one value per column
	 * */

private:
	DATA_FRAME_IDS id;                      // id of Data object
	unsigned long time;                     // timestamp
	std::vector<std::string> columnNames;   // names of data columns
	std::vector<double> values;             // data to hold

public:
	Data(DATA_FRAME_IDS id,
		const unsigned long time,
		const std::vector<std::string> columnNames,
		const std::vector<double> values) {
		/* @brief Constructor for Data object
		 * @param id identifies the data object,
		 * time is a vector of unix timestamps for each row in values
		 * columnNames is a vector of strings to name the value columns
		 * values is a vector of doubles which contains the values
		 * @return returns a instance of the Data class
		 * */
		this->id = id;
		this->time = time;
		this->columnNames = columnNames;
		this->values = values;
	}

	void setId(DATA_FRAME_IDS id) {
		this->id = id;
	}

	DATA_FRAME_IDS getId() {
		return this->id;
	}

	void setTime(const unsigned long time) {
		this->time = time;
	}

	long getTime() {
		return this->time;
	}

	int setColumnNames(const std::vector<std::string> columnNames) {
		if (columnNames.size() == this->values.size()) {
			this->columnNames = columnNames;
			return 1;
		}
		else {
			fprintf(stderr, "DATA Object: columnNames and values length do not agree!");
			return 0;
		}
	}

	std::vector<std::string> getColumnNames() {
		return this->columnNames;
	}

	int setValues(const std::vector<double> values) {
		if (values.size() == this->columnNames.size()) {
			this->values = values;
			return 1;
		}
		else {
			fprintf(stderr, "DATA Object: columnNames and values length do not agree!");
			return 0;
		}
	}

	std::vector<double> getValues() {
		return this->values;
	}

	std::string serialize() {
		std::string archive;
		archive += serializeInteger<uint8_t>(id);
		archive += serializeInteger<unsigned long>(time);
		archive += serializeStringVector(columnNames);
		archive += serializeDoubleVector(values);
		return archive;
	}

	void deserialize(std::string* archive) {
		this->values = deserializeDoubleVector(archive);
		this->columnNames = deserializeStringVector(archive);
		this->time = deserializeInteger<unsigned long>(archive);
		this->id = (DATA_FRAME_IDS)deserializeInteger<uint8_t>(archive);
	}

};


#endif
