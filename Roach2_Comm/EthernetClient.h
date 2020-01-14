#pragma once
#include "Connection.h"

#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_super.h"
#include "../Roach2_Control/Defines.h"

#include "clientsock.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <queue>

// Threading related
#include <thread>
#include <deque>
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <math.h>
#include <sstream>

class EthernetClient :
	public Connection
{
private:
	bool connected;
	int port_number = 12728;
	std::string ip;
	std::mutex access_send_queue;
	std::atomic<bool> stop_running;
	ClientSock* socket;
	std::queue<std::string> send_queue;
	PLATFORM origin;
public:
	EthernetClient(std::string ip, PLATFORM _origin);
	int whichConnection();
	bool isConnected();
	void run();
	void send(std::unique_ptr<Data_super> msg);
	void send(std::string msg);
};

