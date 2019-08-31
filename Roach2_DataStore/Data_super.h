#ifndef DATA_SUPER_HEADER
#define DATA_SUPER_HEADER

#include <cstdint>


/**
* This class is defined as superclass for Data and Data_simple to allow polymorphismus regarding overloading Data
*/
class Data_super
{
public:
	virtual uint64_t* convert_to_serial() = 0;
	virtual int convert_to_serial_array_length() = 0;
};

#endif