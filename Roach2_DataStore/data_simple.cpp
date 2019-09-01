#include "data_simple.h"

Data_simple::Data_simple(uint32_t cmd, uint32_t para)
{
	this->command = cmd;
	this->parameter = para;
}

uint64_t* Data_simple::convert_to_serial()
{
	uint64_t* data = new uint64_t[1];
	data[0] = ((uint64_t)this->command << 32);
	data[0]= data[0] + this->parameter;
	return data;
}

int Data_simple::convert_to_serial_array_length()
{
	return 1;
}

