#ifndef COMMAND_PARSER_HEADER_FILE
#define COMMAND_PARSER_HEADER_FILE

#include "../Roach2_DataStore/Data_super.h"
#include <cstdint>

enum class COMMAND {
	unknown = 0,
	rcu_check_alive,
	gs_check_alive,
	obc_check_alive,
	obc_restart_rover,
    obc_restart_obc,
	obc_rocket_signal_status,
	obc_sim_control,
	obc_state_change,
    obc_rcu_off,
    obc_rcu_on,
    obc_read_sensor,
	rcu_state_change,
	rcu_lights_on,
	rcu_lights_off,
    rcu_hv_on,
    rcu_hv_off,
    rcu_drive_enable,
    rcu_drive_stop,

	NUMBER_ELEMENTS
};

class CommandParser
{
public:
    static COMMAND parse(uint16_t command);
    static DATA_TYPE packet_type(uint64_t packet);
    static uint32_t get_parameter(uint64_t packet);
    static uint16_t get_command(uint64_t packet);
    static PLATFORM get_destination(uint64_t packet);
};

#endif