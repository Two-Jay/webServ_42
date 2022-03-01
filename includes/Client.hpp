#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <string>

class Client
{
private:
	std::string request;
	int received;
	
public:
	socklen_t address_length;
	struct sockaddr_storage address;
	int socket;

	Client();
	~Client();
	
	static Client make_client();
	void drop_client();
	const char *get_client_address();
	const char *get_client_serv();
};

#endif