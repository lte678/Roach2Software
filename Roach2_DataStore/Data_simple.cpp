#include "Data_simple.h"

Data_simple::Data_simple(uint32_t cmd, uint32_t para)
{
	this->command = cmd;
	this->parameter = para;
}

Data_simple::Data_simple(std::string msg) {
	this->msg = msg;
}

Data_simple::~Data_simple() {

}

std::vector<uint64_t> Data_simple::convert_to_serial()
{
	std::vector<uint64_t> data;
	data.push_back(0);
	data[0] = data[0] & ((uint64_t)command << 32);
	data[0] = data[0] & parameter;
	return data;
}

int Data_simple::convert_to_serial_array_length()
{
	return 1;
}

std::string Data_simple::get_string_ethernet()
{
	return msg;
}

