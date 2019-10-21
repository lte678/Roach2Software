/* @file data.cpp
 * @date 05.06.2019
 * @brief Definition of the Data structure class used in the Roach2 experiment for
 * communication and saving.
 *
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "data.h"
#include "serialization.h"

/*
 * 1. constructors
 * */

Data::Data(){
    this->time = 0;
}

Data::Data(int id,
           unsigned long time,
           std::vector<std::string> columnNames,
           std::vector<double> values){
    this->id = id;
    this->time = time;
    this->columnNames = columnNames;
    this->values = values;
}

/*
 * 2. public methods
 * */

void Data::setId(int id)
{
	this->id = id;
}

int Data::getId(){
    return this->id;
}

void Data::setTime(const unsigned long time){
    this->time = time;
}

long Data::getTime(){
    return this->time;
}

void Data::setColumnNames(const std::vector<std::string> columnNames){
    this->columnNames = columnNames;
}

std::vector<std::string> Data::getColumnNames(){
    return this->columnNames;
}

void Data::amendColumnName(std::string name)
{
	this->columnNames.push_back(name);
}

void Data::setValues(const std::vector<double> values){
    this->values = values;
}

std::vector<double> Data::getValues(){
    return this->values;
}

void Data::amendValue(double value)
{
	this->values.push_back(value);
}

/**
 * @brief Adds a value with a type identifier (e.g. type=IMU_acc, value=10.0)
 * @param type: name/identifier of the added value
 * @param value: Added value with double precision (cast all integer/floating point values to double)
*/
void Data::addValue(std::string type, double value)
{
	this->amendColumnName(type);
	this->amendValue(value);
}

std::string Data::serialize(){
    std::string archive;
    //archive += serializeString(id);
    archive += serializeInteger<unsigned long>(time);
    archive += serializeStringVector(columnNames);
    archive += serializeDoubleVector(values);
    return archive;
}

void Data::deserialize(std::string * archive){
    this->values      = deserializeDoubleVector(archive);
    this->columnNames = deserializeStringVector(archive);
    this->time        = deserializeInteger<unsigned long>(archive);
    //this->id          = deserializeString(archive);
}

bool Data::is_valid(){
    /* @brief short self check to see if data is valid and
     * there are as many columnNames as values
     * */
    if(this->id >= 0 || this->columnNames.size()!=this->values.size()){
        return false;
    }else{
        return true;
    }
}

void Data::print(){
    /* @brief prints data contents to the screen
     * */
    std::cout << "ID: " << this->id << std::endl;
    std::cout << "TIME: " << this->time << std::endl;
    std::cout << "COLUMNNAMES: ";
    for(int i = 0; i<this->columnNames.size(); i++) std::cout << this->columnNames[i] << " ";
    std::cout << std::endl;
    std::cout << "VALUES: ";
    for(int i = 0; i<this->values.size(); i++) std::cout << this->values[i] << " ";
    std::cout << std::endl;
}

/**
 * @brief Returns the sensor information converted in uint64 array for transmission over UART.
 *		  Note: The length of the returned array can be requested from convert_to_serial_array_length() function
 * @return uint64_t array
*/
uint64_t* Data::convert_to_serial() {
	uint64_t* packages_struct;
	packages_struct = new uint64_t[50];
	packages_struct[0] = 1; // Length
	// For sensor data: sensor_id => 4bit, sensor_sub_type => 4bit, data => 56bit, MSB first (sensor_id)
	
	if (this->id == SENSOR_TYPES::TEMP_SENSOR) {
		packages_struct[0] = (this->id << 60); // imu sensor has no subtypes
	}
	else if (this->id == SENSOR_TYPES::SYS_INFO) {
		packages_struct[0] = ((uint64_t)this->id << 60); // Important, the order of values in std::vector is physMem, pyhsMemUsed, cpuLoad
		packages_struct[0] += ((uint64_t)1 << 56); // Subtype phyMem
		packages_struct[0] += (uint64_t)(this->values[0]); // physMem
		packages_struct[1] = ((uint64_t)this->id << 60);
		packages_struct[1] += ((uint64_t)2 << 56); // Subtype physMemUsed
		packages_struct[1] += (uint64_t)(this->values[1]); // physMemUsed
		packages_struct[2] = ((uint64_t)this->id << 60);
		packages_struct[2] += ((uint64_t)3 << 56); // Subtype cpuLoad
		packages_struct[2] += (uint64_t)(this->values[2]); // cpuLoad
		packages_struct[3] = ((uint64_t)this->id << 60);
		packages_struct[3] += ((uint64_t)3 << 56); // Subtype temp
		packages_struct[3] += (uint64_t)(this->values[3]); // temp
	}
	return packages_struct;
}

int Data::convert_to_serial_array_length() {
	return this->values.size();
}

