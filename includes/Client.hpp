#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <netdb.h>
#include "Server.hpp"

class Client
{
private:
	int socket;
	int received_size;
	Server *server;
	
public:
	socklen_t address_length;
	struct sockaddr_storage address;
	char request[MAX_REQUEST_SIZE + 1];

	Client(Server *server);
	~Client();
	
	int get_socket() const;
	int get_received_size() const;
	void set_socket(int value);
	void set_received_size(int size);

	std::string get_root_path();
	
	const char *get_client_address();
	const char *get_client_port();
};

#endif