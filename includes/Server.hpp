#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <vector>
#include "DataStructs.hpp"
#include "Location.hpp"

class Server
{
private:
	int client_body_limit;
	bool autoindex;
	std::string host;
	std::string root;
	std::string server_name;
	std::vector<std::string> port;
	std::vector<int> fd;//??
	std::vector<std::string> index;
	std::vector<MethodType> allow_methods;
	std::vector<Location> locations;

public:
	std::vector<int> listen_socket;
	
	Server();
	~Server();

	void create_socket();
};

#endif