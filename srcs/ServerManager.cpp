#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(std::vector<Server> servers)
{
	this->servers = servers;
}

ServerManager::~ServerManager()
{
}

void ServerManager::create_servers()
{
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].create_socket();
	}
}

void ServerManager::wait_on_clients()
{
	int max = -1;
	fd_set reads;

	FD_ZERO(&reads);
	for (int i = 0; i < servers.size(); i++)
	{
		for (int j = 0; j < servers[i].listen_socket.size(); j++)
		{
			FD_SET(servers[i].listen_socket[j], &reads);
			if (max < servers[i].listen_socket[j])
				max = servers[i].listen_socket[j];
		}
	}
	
	for (int i = 0; i < clients.size(); i++)
	{
		FD_SET(clients[i].socket, &reads);
		if (clients[i].socket > max)
			max = clients[i].socket;
	}

	if (select(max + 1, &reads, 0, 0, 0) < 0)
	{
		fprintf(stderr, "select() failed. (%d)\n", errno);
		exit(1);
	}
    //변화가 생긴 소켓
	this->reads = reads;
}

void ServerManager::accept_sockets()
{
	int server;
	for (int i = 0; i < servers.size(); i++)
	{
		for (int j = 0; j < servers[i].listen_socket.size(); j++)
		{
			server = servers[i].listen_socket[j];
			if (FD_ISSET(server, &reads))
			{
				
				clients.push_back(Client::make_client());
				Client &client = clients.back();
				client.socket = accept(server, (struct sockaddr*)&(client.address), &(client.address_length));
				std::cout << "client->socket: " << client.socket << "\n";
				if (client.socket < 0)
				{
					fprintf(stderr, "accept() failed. (%d)\n", errno);
					exit(1);
				}
				printf("New Connection from %s.\n", client.get_client_address());
			}
		}
	}
}