#ifndef SERVER_HPP
#define SERVER_HPP

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
	std::vector<int> port;
	std::vector<int> fd;
	std::vector<std::string> index;
	std::vector<MethodType> allow_methods;
	std::vector<Location> locations;

public:
	Server();
	~Server();

	void createSocket();
};

#endif