#ifndef HEADER_FSM_RCU
#define HEADER_FSM_RCU


#include "../Roach2_Control/FSM_Controller.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_DataStore/data.h"
#include "../Roach2_Control/CommandParser.h"
#include "../Roach2_Comm/UART.h"
#include "../Roach2_Actuators/pwm_pca9658.h"
#include "../Roach2_Actuators/Roach2_Actuators_Actuator_HV.h"
#include "../Roach2_DataStore/Data_super.h"
#include <chrono>
#include <string>

class FSM_RCU :
	public FSM_Controller,
	public ReceiveHandler
{
private:
	PWM_PCA985 *pwm;
	Actuator_HV *hv;
    std::vector<SensorType> sensor_ids; // Sensors to send updates for
    std::chrono::high_resolution_clock::time_point startTime;

    void sensorDownlink();
public:
	FSM_RCU();
	~FSM_RCU();
	void run(void);
	void triggerActuators(void);
	void packageReceivedUART(uint64_t message, int msg_length);
	void rocketSignalReceived(int signal_source);
	void packageReceivedRexus(uint64_t message, int msg_length);
	void packageReceivedEthernet();
	void packageReceivedEthernet_msg(const std::string& command);
	void simulationModeUpdate();
};

#endif
