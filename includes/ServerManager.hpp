#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <sys/time.h>
#include "Server.hpp"

class ServerManager
{
private:
	std::vector<Server> servers;
	int max_fd;
	struct timeval timeout;

	ServerManager();

public:
	ServerManager(std::vector<Server> servers);
	~ServerManager();

	void createServer();
	void run();
};

#endif