#include "CommandParser.h"

/**
 * @brief Parses the given command into a valid command and detects which type of command.
 * Note: Sending a OBC command to RCU and vice versa has no effect, as the command is ignored by the controller.
 * @param command: raw 32bit command word
 * @return parsed command in parse_command_type strucutre
*/
parse_command_type CommandParser::parse(uint16_t command)
{
	parse_command_type result = {};

	// Command type is MSB byte
	// first 4 bits
	uint8_t cmdType = ((command & 0xF000u) >> 12u);

	// Command number
	// last 12 bits
	uint16_t cmdNr = command & 0x0FFFu;

	if (cmdType == 0) {
		// Operational related command

		// Check if valid command
		if (cmdNr > (int)COMMANDS_OPERATIONAL::NUMBER_ELEMENTS - 1) {
			// No valid command
			result.op = COMMANDS_OPERATIONAL::no;
		} else {
			result.op = static_cast<COMMANDS_OPERATIONAL>(cmdNr);
		}
		result.debug = COMMANDS_DEBUG::no;
		result.sim = COMMANDS_SIM::no;
	}
	else if (cmdType == 1) {
		// Debug related command

		// Check if valid command
		if (cmdNr > (int)COMMANDS_DEBUG::NUMBER_ELEMENTS - 1) {
			// No valid command
			result.debug = COMMANDS_DEBUG::no;
		} else {
			result.debug = static_cast<COMMANDS_DEBUG>(cmdNr);
		}
		result.op = COMMANDS_OPERATIONAL::no;
		result.sim = COMMANDS_SIM::no;
	}
	else if (cmdType == 2) {
		// Sim related command

		// Check if valid command
		if (cmdNr > (int)COMMANDS_SIM::NUMBER_ELEMENTS - 1) {
            // No valid command
            result.sim = COMMANDS_SIM::no;
        } else {
			result.sim = static_cast<COMMANDS_SIM>(cmdNr);
		}
		result.op = COMMANDS_OPERATIONAL::no;
		result.debug = COMMANDS_DEBUG::no;
	}

	return result;
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
