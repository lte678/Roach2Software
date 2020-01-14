#include "EthernetClient.h"


EthernetClient::EthernetClient(std::string ip, PLATFORM _origin)
{
    origin = _origin;
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
    std::cout << "[Ethernet Client] Started thread. Connecting to " << ip << ":" << port_number << std::endl;
	this->stop_running.store(false);

	// Connected to server
	while (!connected && !this->stop_running.load()) {
		try {
			this->socket = new ClientSock(this->ip, this->port_number);
			this->access_send_queue.lock();
			this->connected = true;
			this->access_send_queue.unlock();
		}
		catch (SockExcept & e) {
			this->access_send_queue.lock();
			this->connected = false;
			this->access_send_queue.unlock();
		}

		usleep(100);
	}

	// Send messages to server (server is not expected to return data)
	while (!this->stop_running.load()) {
		try {
			this->access_send_queue.lock();
			for (int i = 0; i < this->send_queue.size(); i++) {
				std::string msg = this->send_queue.front();
				*(this->socket) << msg;
				this->send_queue.pop();
			}
			this->access_send_queue.unlock();
		}
		catch (SockExcept &e) {
			// Some error occurred, try to reconnect
			this->access_send_queue.lock();
			this->connected = false;
			this->access_send_queue.unlock();
            std::cout << "[Ethernet Client] Disconnected!" << std::endl;
			try {
				// First try to close socket in case this is not a loose of connection
				this->socket->close();
			}
			catch (SockExcept & e) {
				// Connection already close, this is fine, will try to reconnect next
			}

			// Delete client socket object
			delete this->socket;

			// Try to reconnect
			while (!this->stop_running.load() && !this->connected) {
				try {
					this->socket = new ClientSock(this->ip, this->port_number);
                    std::cout << "[Ethernet Client] Reconnected!" << std::endl;
					this->access_send_queue.lock();
					this->connected = true;
					this->access_send_queue.unlock();
				}
				catch (SockExcept & e) {
				    std::cout << e.get_SockExcept() << std::endl;
					this->access_send_queue.lock();
					this->connected = false;
					this->access_send_queue.unlock();
				}
				usleep(10000);
			}
		}
		usleep(1000);
	}
}


void EthernetClient::send(std::unique_ptr<Data_super> msg) {
    std::vector<uint64_t> packets = msg->convert_to_serial(origin);
    std::stringstream msg_string;
    msg_string << "bin:";
    for (uint64_t packet : packets) {
        msg_string << std::setw(16) << std::setfill('0') << std::hex << packet;
    }
    msg_string << ";";

	this->access_send_queue.lock();
	this->send_queue.push(msg_string.str());
	this->access_send_queue.unlock();
}

void EthernetClient::send(std::string msg) {
    this->access_send_queue.lock();
    this->send_queue.push("txt:" + msg + ";");
    this->access_send_queue.unlock();
}