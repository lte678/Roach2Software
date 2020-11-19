#ifndef HEADER_DATA_SIMPLE
#define HEADER_DATA_SIMPLE

#include "data.h"
#include <vector>
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
		~Data_simple();
		std::vector<uint64_t> convert_to_serial(PLATFORM origin) override;
		int convert_to_serial_array_length();
		std::string get_string_ethernet();
};

#endif
