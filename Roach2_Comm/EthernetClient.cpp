#include "EthernetClient.h"


EthernetClient::EthernetClient(std::string ip)
{
	connected = false;
	this->ip = ip;
}

int EthernetClient::whichConnection()
{
	return CONNECTION_TYPES::ETHERNET;
}

bool EthernetClient::isConnected()
{
	this->access_send_queue.lock();

	bool conn_bit = this->connected;

	this->access_send_queue.unlock();

	return conn_bit;
}

void EthernetClient::run()
{
	// Start thread
	this->stop_running.store(false);

	// Connected to server
	while (!connected && !this->stop_running.load()) {
		try {
			this->socket = new ClientSock(this->ip, this->port_number);
			connected = true;
		}
		catch (SockExcept & e) {
			
		}

		usleep(100);
	}

	while (!this->stop_running.load()) {
		try {

			this->access_send_queue.lock();

			for (int i = 0; i < this->send_queue.size(); i++) {
				std::string msg = this->send_queue.front();
				cout << "Send message: " << msg << "\n";
				*(this->socket) << msg;
				this->send_queue.pop();
			}

			this->access_send_queue.unlock();
		}
		catch (SockExcept &e) {
			cout << "Error: " << e.get_SockExcept() << endl;
		}

		usleep(10);
	}
}


void EthernetClient::send(Data_super* msg) {
	this->access_send_queue.lock();
	
	std::string msg_string = msg->get_string_ethernet();

	this->send_queue.push(msg_string);

	this->access_send_queue.unlock();
}