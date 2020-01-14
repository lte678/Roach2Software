#ifndef COMMAND_PARSER_HEADER_FILE
#define COMMAND_PARSER_HEADER_FILE

#include "../Roach2_DataStore/Data_super.h"
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
	obc_rocket_signal_status = 10,
	obc_sim_control = 11,
	obc_rcu_online = 12,
	obc_obc_state_change = 13,
	obc_rcu_state_change = 14,
	obc_restart_obc = 15,
	obc_eth_stat = 16,
	NUMBER_ELEMENTS = 17
};

enum class COMMANDS_DEBUG {
	no = 0,
	obc_next_state = 1,
	obc_prev_state = 2,
	obc_drive_forward = 3,
	obc_drive_stop = 4,
	rcu_next_state = 5,
	rcu_prev_state = 6,
	obc_sensor_acq_on = 7,
	obc_sensor_acq_off = 8,
	rcu_sensor_acq_on = 9,
	rcu_sensor_acq_off = 10,
	obc_read_sensor = 11,
	obc_light_control = 12,
	obc_gopro_control = 13,
	obc_check_eth = 14,
	rcu_lights_on = 15,
	rcu_lights_off = 16,
	obc_sim_control = 17,
    obc_rcu_off = 18,
    obc_rcu_on = 19,
	NUMBER_ELEMENTS = 20
};

enum class COMMANDS_SIM {
	no = 0,
	obc_sods_event = 1,
	obc_touch_down_event = 2,
	obc_soe_event = 3,
	obc_lo_event = 4,
    rcu_hv_on = 5,
    rcu_hv_off = 6,
    rcu_drive_enable = 7,
    rcu_drive_stop = 8,
	NUMBER_ELEMENTS = 9
};

struct parse_command_type {
	COMMANDS_DEBUG debug;
	COMMANDS_SIM  sim;
	COMMANDS_OPERATIONAL op;
};

class CommandParser
{
	public:
		static parse_command_type parse(uint16_t command);
		static DATA_TYPE packet_type(uint64_t packet);
		static uint32_t get_parameter(uint64_t packet);
		static uint16_t get_command(uint64_t packet);
		static PLATFORM get_destination(uint64_t packet);
};

#endif