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
#include <iostream>
#include "serialization.h"

class Data {
    /*
     * @brief Data Structure used for communication and data saving. Every
     * instance can contain one value per column
     * */

    private:
        std::string id;                         // id of Data object
        unsigned long time;                     // timestamp
        std::vector<std::string> columnNames;   // names of data columns
        std::vector<double> values;             // data to hold

    public:
        // constructors
        Data();
        Data(std::string id,
             unsigned long time,
             std::vector<std::string> columnNames,
             std::vector<double> values);

        // set & get
        void setId(int id);
        std::string getId();

        void setTime(const unsigned long time);
        long getTime();

        void setColumnNames(const std::vector<std::string> columnNames);
        std::vector<std::string> getColumnNames();
		void amendColumnName(std::string name);

        void setValues(const std::vector<double> values);
        std::vector<double> getValues();
		void amendValue(double value);

        // serialize & deserialize
        std::string serialize();
        void deserialize(std::string * archive);

        // self check if valid data
        bool is_valid();

        // pretty print
        void print();

};


#endif
