#ifndef DATASTRUCTS_HPP
#define DATASTRUCTS_HPP

#include <sys/socket.h>
#include <string>

enum MethodType
{
	GET,
	POST,
	DELETE,
};

struct client_info
{
	socklen_t address_length;
	struct sockaddr_storage address;
	int socket;
	std::string request;
	int received;
	struct client_info *next;
};

#endif