#ifndef DATA_SUPER_HEADER
#define DATA_SUPER_HEADER

#include <cstdint>
#include <string>
#include <vector>


/**
* This class is defined as superclass for Data and Data_simple to allow polymorphismus regarding overloading Data
*/
class Data_super
{
public:
    virtual ~Data_super() = default;
	virtual std::vector<uint64_t> convert_to_serial() = 0;
	virtual int convert_to_serial_array_length() = 0;
	virtual std::string get_string_ethernet() = 0;
};

#endif