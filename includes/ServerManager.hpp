#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <iostream>
#include <sys/time.h>
#include "Server.hpp"
#include "Client.hpp"

class ServerManager
{
private:
	std::vector<Server> servers;
	std::vector<Client> clients;
	int max_fd;
	fd_set reads;
	struct timeval timeout;

	ServerManager();

public:
	ServerManager(std::vector<Server> servers);
	~ServerManager();

	void create_servers();
	void wait_on_clients();
	void accept_sockets();

	void run();
};

#endif