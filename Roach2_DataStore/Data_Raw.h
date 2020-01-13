//
// Created by leon on 12.01.20.
//

#ifndef ROACH2_SOFTWARE_DATA_RAW_H
#define ROACH2_SOFTWARE_DATA_RAW_H

#include <vector>

#include "Data_super.h"

class Data_Raw : public Data_super {
private:
    std::vector<uint64_t> data;
public:
    Data_Raw();
    ~Data_Raw();
    void addElement(uint64_t element);
    std::vector<uint64_t> convert_to_serial(PLATFORM origin) override;
    int convert_to_serial_array_length() override;
    std::string get_string_ethernet() override { return ""; };
};


#endif //ROACH2_SOFTWARE_DATA_RAW_H
