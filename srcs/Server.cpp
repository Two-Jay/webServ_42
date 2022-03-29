#include "../includes/Server.hpp"

Server::Server(/* args */)
{
	client_body_limit = 1024;
	autoindex = true;
	host = "";
	redirect_status = -1;
}

Server::~Server()
{
}

void Server::create_socket()
{
	printf("===Server::createSocket===\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_addr;
	for (int i = 0;i < port.size();i++) {
		memset(bind_addr, 0, sizeof(struct addrinfo*));
		getaddrinfo(host.c_str(), port[i].c_str(), &hints, &bind_addr);

		printf("Creating socket...\n");
		int socket_listen = socket(bind_addr->ai_family,
				bind_addr->ai_socktype, bind_addr->ai_protocol);
		fcntl(socket_listen, F_SETFL, O_NONBLOCK);
		if (socket_listen < 0) {
			fprintf(stderr, "socket() failed. (%d)\n", errno);
			exit(1);
		}
		printf("Binding socket to local address...\n");
		if (bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen)) {
			fprintf(stderr, "bind() failed. (%d)\n", errno);
			perror("bind");
			exit(1);
		}
		freeaddrinfo(bind_addr);
		printf("Listening...\n");
		if (listen(socket_listen, 10) < 0) {
			fprintf(stderr, "listen() failed. (%d)\n", errno);
			exit(1);
		}
		this->listen_socket.push_back(socket_listen);
		printf("added!\n");
	}
}

void Server::print_server_info()
{
	std::cout << "------------------ Server Info ------------------" << std::endl;
	std::cout << "> server_name: " << server_name << std::endl;
	std::cout << "> host: " << host << std::endl;
	std::cout << "> port: " << port << std::endl;
	if (redirect_status == -1)
	{
		std::cout << "> root: " << root << std::endl;
		std::cout << "> client_body_limit: " << client_body_limit << std::endl;
		std::cout << "> autoindex: " << (autoindex ? "on" : "off") << std::endl;
		std::cout << "> index: " << index << std::endl;
		std::cout << "> allow_methods: " << allow_methods << std::endl;
		for (int i = 0; i < locations.size(); i++)
		{
			locations[i].print_location_info();
		}
	}
	else
	{
		std::cout << "> redirect_status: " << redirect_status << std::endl;
		std::cout << "> redirect_url: " << redirect_url << std::endl;
	}
	std::cout << "-------------------------------------------------" << std::endl;
}

MethodType Server::s_to_methodtype(std::string str)
{
	if (str == "GET")
	{
		return GET;
	}
	else if (str == "POST")
	{
		return POST;
	}
	else if (str == "DELETE")
	{
		return DELETE;
	}
	return INVALID;
}