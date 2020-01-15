#include "CommandParser.h"

/**
 * @brief Parses the given command into a valid command and detects which type of command.
 * Note: Sending a OBC command to RCU and vice versa has no effect, as the command is ignored by the controller.
 * @param command: raw 32bit command word
 * @return parsed command in parse_command_type strucutre
*/
COMMAND CommandParser::parse(uint16_t command)
{
    if (command < (uint16_t)COMMAND::NUMBER_ELEMENTS) {
        return static_cast<COMMAND>(command);
    } else {
        return COMMAND::unknown;
    }
}

DATA_TYPE CommandParser::packet_type(uint64_t packet) {
    uint64_t typeCode = (packet & 0x0F00000000000000u) >> 56u;
    switch(typeCode) {
        case (uint64_t)DATA_TYPE::COMMAND:
            return DATA_TYPE::COMMAND;
        case (uint64_t)DATA_TYPE::SENSOR_DATA:
            return DATA_TYPE::SENSOR_DATA;
        default:
            return DATA_TYPE::UNKNOWN;
    }
}

uint32_t CommandParser::get_parameter(uint64_t packet) {
    return (uint32_t)(packet & 0xFFFFFFFF);
}

PLATFORM CommandParser::get_destination(uint64_t packet) {
    uint64_t dest = (packet & 0xF000000000000000u) >> 60u;
    switch(dest) {
        case (uint64_t)PLATFORM::OBC:
            return PLATFORM::OBC;
        case (uint64_t)PLATFORM::RCU:
            return PLATFORM::RCU;
        default:
            return PLATFORM::UNKNOWN;
    }
}

uint16_t CommandParser::get_command(uint64_t packet) {
    // Das Command ist im 2. und 3. Byte gespeichert
    return (uint16_t)((packet & 0x00FFFF0000000000u) >> 40u);
}
