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
    return data;
}

int Data_Raw::convert_to_serial_array_length() {
    return data.size();
}