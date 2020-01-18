//
// Created by leon on 12.01.20.
//

#include "Data_Raw.h"

Data_Raw::Data_Raw() {

}

Data_Raw::~Data_Raw() {

}

void Data_Raw::addElement(uint64_t element) {
    data.push_back(element);
}

std::vector<uint64_t> Data_Raw::convert_to_serial(PLATFORM origin) {
    if(!data.empty()) {
        return data;
    } else {
        std::cout << "[Data Raw] No data. Returning 0 element" << std::endl;
        return std::vector<uint64_t >({0}); // At least have some data to work with. Empty vector might cause problems
    }

}

int Data_Raw::convert_to_serial_array_length() {
    if(!data.empty()) {
        return data.size();
    } else {
        return 1;
    }
}