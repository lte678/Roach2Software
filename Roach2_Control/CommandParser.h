#ifndef COMMAND_PARSER_HEADER_FILE
#define COMMAND_PARSER_HEADER_FILE

#include <cstdint>
enum class COMMANDS_OPERATIONAL {
	no = 0,
	rcu_check_alive = 1,
	gs_check_alive = 2,
	obc_check_alive = 3,
	rcu_sods_event = 4,
	rcu_lo_event = 5,
	rcu_soe_event = 6,
	obc_restart_rover = 7,
	rcu_touch_down_event = 8,
	rcu_read_sensor  = 9,
	obc_rcu_online = 11,
	obc_rocket_signal_status = 10,
	NUMBER_ELEMENTS = 12
};

enum class COMMANDS_DEBUG {
	no = 0,
	obc_next_state = 1,
	obc_prev_state = 2,
	obc_drive_forward = 3,
	obc_drive_stop = 4,
	rcu_drive_forward = 5,
	rcu_next_state = 6,
	rcu_prev_state = 7,
	rcu_drive_stop = 8,
	rcu_hv_on = 9,
	rcu_hv_off = 10,
	obc_rcu_off = 11,
	obc_rcu_on = 12,
	obc_sensor_acq_on = 13,
	obc_sensor_acq_off = 14,
	rcu_sensor_acq_on = 15,
	rcu_sensor_acq_off = 16,
	obc_read_sensor = 17,
	NUMBER_ELEMENTS = 18
};

enum class COMMANDS_SIM {
	no = 0,
	obc_sods_event = 1,
	obc_touch_down_event = 2,
	obc_soe_event = 3,
	obc_lo_event = 4,
	NUMBER_ELEMENTS = 5
};

struct parse_command_type {
	COMMANDS_DEBUG debug;
	COMMANDS_SIM  sim;
	COMMANDS_OPERATIONAL op;
};

class CommandParser
{
	public:
		static parse_command_type parse(uint32_t command);
};

#endif