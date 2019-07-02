/* @file FSM_OBC.h
 * @date 30.06.2019
 * @brief FSM controller implementation for OBC
 *
 * @author Matthias Burk
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef HEADER_FSM_OBC
#define HEADER_FSM_OBC

#include "..\Roach2_Control\FSM_Controller.h"
#include <chrono>

class FSM_OBC :
	public FSM_Controller
{
public:
	FSM_OBC();
	~FSM_OBC();
	void run(void);
};

#endif

