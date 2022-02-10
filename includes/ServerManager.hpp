#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"

class ServerManager
{
private:
	std::vector<Server> servers;


public:
	ServerManager(/* args */);
	~ServerManager();

	void initServer();
};

#endif