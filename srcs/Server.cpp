#include "../includes/Server.hpp"

Server::Server(/* args */)
{
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
	}
}