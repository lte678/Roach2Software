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

bool EthernetServer::isDataReceived()
{
	this->access_receive_queue.lock();

	bool empty_bit = this->receive_queue.empty();

	this->access_receive_queue.unlock();

	return !empty_bit;
}

bool EthernetServer::isConnected()
{
	this->access_receive_queue.lock();
	bool conn_bit = this->connected;
	this->access_receive_queue.unlock();

	return conn_bit;
}

std::vector<std::string> EthernetServer::getReceivedValues()
{
	std::vector<std::string> copied;
	this->access_receive_queue.lock();

	// Copies all received string to vector and deletes the queue content
	while (!this->receive_queue.empty()) {
		std::string element = this->receive_queue.front();
		copied.push_back(element);
		this->receive_queue.pop();
	}

	this->access_receive_queue.unlock();
	return copied;
}

void EthernetServer::run()
{
    std::cout << "[Ethernet Server] Started thread. Listening on " << port_number << std::endl;
	std::string message;
	ServerSock new_conn;

	// Start thread
	this->stop_running.store(false);

	// Connect to client, wait unitl connection established or thread stopped
	while (this->stop_running.load() || !connected) {
		try {
			this->socket->accept(new_conn);
			this->access_receive_queue.lock();
			this->connected = true;
			this->access_receive_queue.unlock();
		}
		catch (SockExcept & e) {
			this->access_receive_queue.lock();
			this->connected = false;
			this->access_receive_queue.unlock();
		}
	}

	// Receive data until the end
	while (!this->stop_running.load()) {
		try {
			new_conn >> message;
			this->access_receive_queue.lock();
			this->receive_queue.push(message);
			this->access_receive_queue.unlock();
		}
		catch (SockExcept & e) {
			// Some error occurred, try to reconnect
			this->access_receive_queue.lock();
			this->connected = false;
			this->access_receive_queue.unlock();

			try {
				// First try to close socket in case this is not a loose of connection
				this->socket->close();
			}
			catch (SockExcept & e) {
				// Connection already close, this is fine, will try to reconnect next
			}

			// Now try to reconnect (accept incoming connection)
			while (this->stop_running.load() || !this->connected) {
				try {
					this->socket->accept(new_conn);
					this->access_receive_queue.lock();
					this->connected = true;
					this->access_receive_queue.unlock();
				}
				catch (SockExcept & e) {
					this->access_receive_queue.lock();
					this->connected = false;
					this->access_receive_queue.unlock();
				}
			}
		}
	}
}
