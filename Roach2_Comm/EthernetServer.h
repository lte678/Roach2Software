#pragma once
#include "Connection.h"

#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "serversock.h"

#include <string>
#include <iostream>
#include <vector>
#include <queue>

// Threading related
#include <mutex>
#include <atomic>

class EthernetServer :
	public Connection
{
private:
    static constexpr int port_number = 12728;
    static constexpr char delimiter = '\0';

	ServerSock* socket;
	std::atomic<bool> connected;
	std::mutex access_receive_queue;
	std::atomic<bool> stop_running;
	std::queue<std::string> receive_queue;
public:
	EthernetServer();
	int whichConnection();
	bool isDataReceived();
	bool isConnected();
	std::string popMessage();
	void run();
};

