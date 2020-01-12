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
	while (!receive_queue.empty()) {
		copied = receive_queue.front();
		receive_queue.pop();
	}
	access_receive_queue.unlock();

	return copied;
}

void EthernetServer::run()
{
    std::cout << "[Ethernet Server] Started thread. Listening on " << port_number << std::endl;
	std::string buffer;
	ServerSock new_conn;

	// Start thread
	stop_running = false;

	// Connect to client, wait unitl connection established or thread stopped
	while (!stop_running && !connected) {
		try {
			socket->accept(new_conn);
			connected = true;
		}
		catch (SockExcept & e) {
			connected = false;
		}
	}

	// Receive data until the end
	while (!stop_running) {
		try {
            std::string message;
			new_conn >> message;

			size_t pos = 0;
			std::string token;
			while((pos = message.find(delimiter)) != std::string::npos) {
			    token = message.substr(0, pos);
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

			try {
				// First try to close socket in case this is not a loose of connection
				socket->close();
			}
			catch (SockExcept & e) {
				// Connection already close, this is fine, will try to reconnect next
			}

			// Now try to reconnect (accept incoming connection)
			while (stop_running.load() || !connected) {
				try {
					socket->accept(new_conn);
					connected = true;
				}
				catch (SockExcept & e) {
					connected = false;
				}
			}
		}
	}
}
