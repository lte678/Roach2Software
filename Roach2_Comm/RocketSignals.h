#ifndef ROCKET_SIGNALS_HEADER_FILE
#define ROCKET_SIGNALS_HEADER_FILE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <thread>
#include <atomic>
#include <math.h>
#include <mutex>

#include "../Roach2_Comm/ReceiveHandler.h"
#include "../Roach2_Comm/Connection.h"

class RocketSignals :
	public Connection
{
private:
	bool sods;
	bool soe;
	bool lo;
	int fd_sods;
	int fd_soe;
	int fd_lo;
	std::atomic<bool> changed;
	std::atomic<bool> running;
	std::mutex access_limit;
	ReceiveHandler *handler;
	void initGpio();
	bool checkGpio();
	double sig_lo_acc;
	double sig_sods_acc;
	double sig_soe_acc;
	int sig_counter;
	const int max_sig_counter = 100;
public:
	RocketSignals();
	int whichConnection(void);
	void run();
	void stop();
	bool signalChanged();
	bool* getRocketSignals();
};

#endif

