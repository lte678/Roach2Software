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

std::vector<uint64_t> Data_simple::convert_to_serial(PLATFORM origin)
{
	std::vector<uint64_t> data;
	data.push_back(0);
    // Data structure:
    // |4bit|4bit| 1 byte |     2 bytes    |             4 bytes            |
    // |from|type|command |      empty     |             payload            |

    data[0] += ((uint64_t)origin & 0xFu) << 60u; // from
    data[0] += ((uint64_t)DATA_TYPE::COMMAND & 0xFu) << 56u; // data type
    data[0] += ((uint64_t)command & 0xFFu) << 48u; // command
    data[0] += parameter; // Parameter
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

