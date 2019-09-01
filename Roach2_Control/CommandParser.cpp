#include "CommandParser.h"

/**
 * @brief Parses the given command into a valid command and detects which type of command.
 * Note: Sending a OBC command to RCU and vice versa has no effect, as the command is ignored by the controller.
 * @param command: raw 32bit command word
 * @return parsed command in parse_command_type strucutre
*/
parse_command_type CommandParser::parse(uint32_t command)
{
	parse_command_type result;

	// Command type is MSB byte
	uint8_t msb = ((command & 0xFF000000) >> 28);

	// Command number
	uint32_t nr = command & 0x00FFFFFF;

	if (msb == 0) {
		// Operational related command

		// Check if valid command
		if (nr > (int)COMMANDS_OPERATIONAL::NUMBER_ELEMENTS-1) {
			// No valid command
			result.op = COMMANDS_OPERATIONAL::no;
		}
		else {
			result.op = static_cast<COMMANDS_OPERATIONAL>(nr);
		}
		result.debug = COMMANDS_DEBUG::no;
		result.sim = COMMANDS_SIM::no;
	}
	else if (msb == 1) {
		// Debug related command

		// Check if valid command
		if (nr > (int)COMMANDS_DEBUG::NUMBER_ELEMENTS - 1) {
			// No valid command
			result.debug = COMMANDS_DEBUG::no;
		}
		else {
			result.debug = static_cast<COMMANDS_DEBUG>(nr);
		}
		result.op = COMMANDS_OPERATIONAL::no;
		result.sim = COMMANDS_SIM::no;
	}
	else if (msb == 2) {
		// Sim related command

		// Check if valid command
		if (nr > (int)COMMANDS_SIM::NUMBER_ELEMENTS - 1) {
			// No valid command
			result.sim = COMMANDS_SIM::no;
		}
		else {
			result.sim = static_cast<COMMANDS_SIM>(nr);
		}
		result.op = COMMANDS_OPERATIONAL::no;
		result.debug = COMMANDS_DEBUG::no;
	}

	return result;
}
