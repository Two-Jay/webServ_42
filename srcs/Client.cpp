#include "../includes/Client.hpp"

Client::Client(Server *server)
{
	this->server = server;
	address_length = sizeof(struct sockaddr_storage);
	received_size = 0;
	memset(request, 0, MAX_REQUEST_SIZE + 1);
}

Client::~Client()
{
}

int Client::get_socket() const
{
	return socket;
}

int Client::get_received_size() const
{
	return received_size;
}

void Client::set_socket(int value)
{
	socket = value;
}

void Client::set_received_size(int size)
{
	received_size = size;
}

const char *Client::get_client_address()
{
	static char address_buffer[100];
	getnameinfo((struct sockaddr *)&address, address_length,
		address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
	return address_buffer;
}

const char *Client::get_client_port()
{
	static char service_buffer[100];
	getnameinfo((struct sockaddr *)&address, address_length,
		0, 0, service_buffer, sizeof(service_buffer), NI_NUMERICHOST);
	return service_buffer;
}

std::string Client::get_root_path(std::string path)
{
	std::string root;
	int root_length = 0;
	int index = -1;

	for (int i = 0; i < server->locations.size(); i++)
	{
		if (server->locations[i].root != "" && path.find(server->locations[i].path) != std::string::npos)
		{
			if (root_length < server->locations[i].path.length())
			{
				root_length = server->locations[i].path.length();
				root = server->locations[i].root;
				index = i;
			}
		}
	}
	if (index == -1)
		root = server->root;
	return root;
}