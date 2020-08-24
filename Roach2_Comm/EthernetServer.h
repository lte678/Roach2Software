#pragma once
#include "Connection.h"

#include "../Roach2_DataStore/data.h"
#include "../Roach2_DataStore/Data_simple.h"
#include "../Roach2_DataStore/Data_Raw.h"
#include "serversock.h"

#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <memory>

// Threading related
#include <mutex>
#include <atomic>

class EthernetServer :
	public Connection
{
private:
    static constexpr int port_number = 12728;
    static constexpr char delimiter = ';';

	ServerSock* socket;
	std::atomic<bool> connected;
	std::mutex access_receive_queue;
	std::atomic<bool> stop_running;
	std::queue<std::string> receive_queue;
	std::vector<std::thread> serverThreads;
public:
	EthernetServer();
	int whichConnection();
	bool isDataReceived();
	bool isConnected();
	std::string popMessage();
	void run();
	void clientThread(std::unique_ptr<ServerSock> conn);

	static Data_Raw* parseBinary(std::string msg);
};

