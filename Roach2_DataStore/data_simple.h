#ifndef HEADER_DATA_SIMPLE
#define HEADER_DATA_SIMPLE

#include "data.h"
#include <string>

class Data_simple :
	public Data_super
{
private:
	uint32_t command;
	uint32_t parameter;
	std::string msg;
public: 
		Data_simple(std::string msg);
		Data_simple(uint32_t cmd, uint32_t para);
		uint64_t* convert_to_serial();
		int convert_to_serial_array_length();
		std::string get_string_ethernet();
};

#endif
