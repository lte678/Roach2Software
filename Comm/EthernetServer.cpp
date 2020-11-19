#include "EthernetServer.h"

EthernetServer::EthernetServer()
{
	// Init ethernet server
	connected = false;
	socket = new ServerSock(port_number);
}

int EthernetServer::whichConnection()
{
	return CONNECTION_TYPES::ETHERNET;
}

bool EthernetServer::isDataReceived()
{
	access_receive_queue.lock();
	bool empty = receive_queue.empty();
	access_receive_queue.unlock();

	return !empty;
}

bool EthernetServer::isConnected()
{
	return connected;
}

std::string EthernetServer::popMessage()
{
	std::string copied;

	access_receive_queue.lock();
	// Copies all received string to vector and deletes the queue content
	if (!receive_queue.empty()) {
		copied = receive_queue.front();
		receive_queue.pop();
	}
	access_receive_queue.unlock();

	return copied;
}

void EthernetServer::run()
{
    std::cout << "[Ethernet Server] Started thread. Listening on " << port_number << std::endl;

	// Start thread
	stop_running = false;

	// Connect to client, wait unitl connection established or thread stopped
	while (!stop_running) {
		try {
            std::unique_ptr<ServerSock> new_conn(new ServerSock);
			socket->accept(*new_conn);
			std::thread serverThread(&EthernetServer::clientThread, this, std::move(new_conn));
			serverThreads.push_back(std::move(serverThread));
			connected = true;
		}
		catch (SockExcept & e) {
            usleep(100000); // Dont spam syscall if it crashes every time
		}
	}

	for(std::thread &thread : serverThreads) {
	    thread.join();
	}
}

void EthernetServer::clientThread(std::unique_ptr<ServerSock> conn) {
    static int threadNum = 0;
    threadNum++;
    std::cout << "[Ethernet Server] ID " <<  threadNum << ", New client connection." << std::endl;

    std::string buffer;

    // Receive data until the end
    while (!stop_running) {
        try {
            std::string message;
            (*conn) >> message;

            size_t pos = 0;
            std::string token;
            while((pos = message.find(delimiter)) != std::string::npos) {
                token = message.substr(0, pos);
                message.erase(0, pos + 1);
                access_receive_queue.lock();
                receive_queue.push(buffer + token);
                access_receive_queue.unlock();
                buffer = "";
            }
            buffer += message;
        }
        catch (SockExcept & e) {
            // Some error occurred, try to reconnect
            connected = false;
            std::cout << "[Ethernet Server] ID " <<  threadNum << ", Disconnected!" << std::endl;

            // Now try to reconnect (accept incoming connection)
            while (!stop_running && !connected) {
                try {
                    socket->accept(*conn);
                    connected = true;
                    std::cout << "[Ethernet Server] ID " <<  threadNum << ", Reconnected!" << std::endl;
                }
                catch (SockExcept & e) {
                    connected = false;
                }
            }
        }
    }
}


Data_Raw* EthernetServer::parseBinary(std::string msg) {
    if (msg.substr(0, 4) == "bin:") {
        msg.erase(0, 4);

        Data_Raw *data = new Data_Raw();

        while (msg.length() >= 16) {
            uint64_t packet = strtoull(msg.substr(0, 16).c_str(), nullptr,16);
            data->addElement(packet);
            msg.erase(0, 16);
        }

        return data;
    }
    return nullptr;
}
