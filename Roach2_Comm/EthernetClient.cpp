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

void EthernetClient::run()
{
	// Start thread
	this->stop_running.store(false);

	std::string msg = "TESTNACHRICHT_12343243242343242343243423243242343 \n";

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
				*(this->socket) << this->send_queue.front();
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