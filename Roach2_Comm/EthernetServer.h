#pragma once
#include "Connection.h"

#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "serversock.h"

#include <string>
#include <iostream>

#include <queue>
#include <vector>

// Threading related
#include <thread>
#include <deque>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <math.h>

class EthernetServer :
	public Connection
{
private:
	ServerSock* socket;
	bool connected;
	int port_number = 12728;
	std::mutex access_receive_queue;
	std::atomic<bool> stop_running;
	std::queue<std::string> receive_queue;
public:
	EthernetServer();
	int whichConnection();
	bool isDataReceived();
	std::vector<std::string> getReceivedValues();
	void run();
};

