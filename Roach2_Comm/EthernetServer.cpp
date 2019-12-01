#include "EthernetServer.h"

EthernetServer::EthernetServer()
{
	// Init RCU server
	connected = false;
	this->socket = new ServerSock(port_number);
}

int EthernetServer::whichConnection()
{
	return CONNECTION_TYPES::ETHERNET;
}

void EthernetServer::run()
{
	std::string message;
	ServerSock new_conn;

	// Start thread
	this->stop_running.store(false);

	// Connect to server, wait unitl connection established or thread stopped
	while (this->stop_running.load() || !connected) {
		this->socket->accept(new_conn);
		this->connected = true;
	}

	// Receive data until the end
	while (!this->stop_running.load()) {
		try {
			new_conn >> message;
			// Simple output for test
			cout << "Message received: " << message << "\n";

			this->access_receive_queue.lock();

			this->receive_queue.push(message);

			this->access_receive_queue.unlock();
		}
		catch (SockExcept & e) {
			cout << "Error: " << e.get_SockExcept() << endl;
		}

		usleep(10);
	}

}