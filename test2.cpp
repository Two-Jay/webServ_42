#include "test2.hpp"

const char *get_content_type(const char *path)
{
	const char *last_dot = strrchr(path, '.');
	if (last_dot) {
		if (strcmp(last_dot, ".css") == 0) return "text/css";
		if (strcmp(last_dot, ".csv") == 0) return "text/csv";
		if (strcmp(last_dot, ".html") == 0) return "text/html";
		if (strcmp(last_dot, ".js") == 0) return "application/javascript";
		if (strcmp(last_dot, ".json") == 0) return "application/json";
		if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
		if (strcmp(last_dot, ".gif") == 0) return "image/gif";
		if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
		if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
		if (strcmp(last_dot, ".png") == 0) return "image/png";
		if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
	}
	return "text/plain";
}

int create_socket(const char *host, const char* port)
{
	printf("Configuring local address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_addr;
	getaddrinfo(host, port, &hints, &bind_addr);

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
	return socket_listen;
}

void makeClient()
{
	struct client_info newClient;

	newClient.address_length = sizeof(struct sockaddr_storage);
	clients.push_back(newClient);
	std::cout << "get_client-2\n";
}

void drop_client(struct client_info client)
{
	close(client.socket);

	std::list<client_info>::iterator iter;
	for (iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter).socket == client.socket)
		{
			clients.erase(iter);
			return;
		}
	}
	fprintf(stderr, "drop_client not found.\n");
	exit(1);
}

const char *get_client_address(struct client_info *ci)
{
	static char address_buffer[100];
	getnameinfo((struct sockaddr *)&ci->address,
		ci->address_length,
		address_buffer, sizeof(address_buffer), 0, 0,
		NI_NUMERICHOST);
	return address_buffer;
}

const char *get_client_serv(struct client_info *ci)
{
	static char serv_buffer[100];
	getnameinfo((struct sockaddr *)&ci->address,
		ci->address_length,
		0, 0, serv_buffer, sizeof(serv_buffer),
		NI_NUMERICHOST);
	return serv_buffer;
}

int int_max(std::vector<int> data)
{
	// int size = sizeof(data) / sizeof(int);
    int max = data[0];
    for (int i = 0; i < data.size(); i++)
	{
		if (max < data[i])
			max = data[i];
    }
	return max;
}

fd_set wait_on_clients(std::vector<int> server)
{
	fd_set reads;

	FD_ZERO(&reads);
	for (int i =0; i<server.size();i++)
	{
		FD_SET(server[i], &reads);
	}
	int max_socket = int_max(server);
	
	for (std::list<client_info>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		FD_SET((*iter).socket, &reads);
		if (((*iter).socket > max_socket))
			max_socket = (*iter).socket;
	}

	if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
	{
		fprintf(stderr, "select() failed. (%d)\n", errno);
		exit(1);
	}
    //변화가 생긴 소켓
	return reads;
}

void sendErrorPage(int code, struct client_info client)
{
	std::string result;
	result.append("HTTP/1.1" + std::to_string(code) + "\r\n");
	result.append("Connection: close\r\n");

	const char *c = result.c_str();
	send(client.socket, c, sizeof(c), 0);
	drop_client(client);
}

void send_400(struct client_info client)
{
	const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
					"Connection: close\r\n"
					"Content-Length: 11\r\n\r\nBad Request";
	send(client.socket, c400, sizeof(c400), 0);
	drop_client(client);
}

void send_404(struct client_info client)
{
	const char *c404 = "HTTP/1.1 404 Not Found\r\n"
					"Connection: close\r\n"
					"Content-Length: 9\r\n\r\nNot Found";
	send(client.socket, c404, sizeof(c404), 0);
	drop_client(client);
}

// print the connected client information
void serve_resource(struct client_info *client, const char *path)
{
	printf("serve_resource %s %s\n", get_client_address(client), path);
	if (strcmp(path, "/") == 0) path = "index.html";
	if (strlen(path) > 100)
	{
		sendErrorPage(400, *client);
		return;
	}
	if (strstr(path, ".."))
	{
		sendErrorPage(404, *client);
		return ;
	}
	
	char full_path[128];
	sprintf(full_path, "www/html/%s", path);
	FILE *fp = fopen(full_path, "rb");
	if (!fp)
	{
		sendErrorPage(404, *client);
		return ;
	}
	// file size 계산
	fseek(fp, 0L, SEEK_END);
	size_t cl = ftell(fp);
	rewind(fp);
	// file type 얻기
	const char *ct = get_content_type(full_path);
	char buffer[BSIZE];
	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Connection: close\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Content-Length: %zu\r\n", cl);
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Content-Type: %s\r\n", ct);
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	int r = fread(buffer, 1, BSIZE, fp);
	while (r)
	{
		send(client->socket, buffer, r, 0);
		r = fread(buffer, 1, BSIZE, fp);
	}
	fclose(fp);
	std::cout << "drop client\n";
	drop_client(*client);
}

//post 요청 처리
void serve_resource_p(struct client_info *client, const char *path, char *data)
{
	printf("serve_resource %s\n", get_client_address(client));
	if (strcmp(path, "/") == 0) path = "/index.html";
	if (strlen(path) > 100)
	{
		sendErrorPage(400, *client);
		return;
	}
	if (strstr(path, ".."))
	{
		sendErrorPage(404, *client);
		return ;
	}
	
	char full_path[128];
	sprintf(full_path, "test/1");
	FILE *fp = fopen(full_path, "r");
	if (!fp)
	{
		sendErrorPage(404, *client);
		return ;
	}
	// file size 계산
	fseek(fp, 0L, SEEK_END);
	size_t cl = ftell(fp);
	rewind(fp);
	// file type 얻기
	const char *ct = get_content_type(full_path);
	fclose(fp);
	char buffer[BSIZE];
	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Connection: close\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Content-Length: %zu\r\n", cl);
	send(client->socket, buffer, strlen(buffer), 0);

	sprintf(buffer, "Content-Type: %s\r\n", ct);
	send(client->socket, buffer, strlen(buffer), 0);

	FILE *cookie = fopen("cookies/1", "r");
	if (!cookie)
	{
		cookie = fopen("cookies/1", "a");
		sprintf(buffer, "Set-Cookie: id=1\r\n");
		send(client->socket, buffer, strlen(buffer), 0);
	}
	else
	{
		sprintf(buffer, "Cookie: id=1\r\n");
		send(client->socket, buffer, strlen(buffer), 0);
		fclose(cookie);
		cookie = fopen("cookies/1", "a");
	}
	fclose(cookie);

	sprintf(buffer, "\r\n");
	send(client->socket, buffer, strlen(buffer), 0);

	fp = fopen(full_path, "r");
	int r = fread(buffer, 1, BSIZE, fp);
	while (r)
	{
		send(client->socket, buffer, r, 0);
		r = fread(buffer, 1, BSIZE, fp);
	}
	fclose(fp);
	drop_client(*client);
}

void sendResponse(fd_set reads)
{
	std::list<client_info>::iterator client = clients.begin();
	while (client != clients.end())
	{
		std::cout << "sendResponse-1\n";
		std::cout << "client.socket: " << (*client).socket << "\n";
		if (FD_ISSET((*client).socket, &reads))
		{
			std::cout << "sendResponse-2\n";
			if (MAX_REQUEST_SIZE == (*client).received)
			{
				sendErrorPage(400, *client);
				continue;
			}
			// request에 데이터 채우기
			// response()
			
			// 받은 데이터 크기 체크
			// 이미 받은 데이터 다음위치를 체크해서 받음
			// 최대 사이즈가 MAX 사이즈를 넘지 않게
			int r = recv((*client).socket, (*client).request + (*client).received, MAX_REQUEST_SIZE - (*client).received, 0);
			printf("client->request: %s\n", (*client).request);
			if (r < 1)
			{
				printf("Unexpected disconnect from %s.\n", get_client_address(&(*client)));
				drop_client(*client);
			}
			else
			{
				(*client).received += r;
				(*client).request[(*client).received] = 0;
				char *q = strstr((*client).request, "\r\n\r\n");
				if (q)
				{
					if (strncmp("GET /", (*client).request, 5) == 0)
					{
						std::cout << "GET /\n";
						char *path = (*client).request + 4;
						char *end_path = strstr(path, " ");
						if (!end_path)
						{
							sendErrorPage(400, *client);
						}
						else
						{
							*end_path = 0;
							serve_resource(&(*client), path);
						}
					}
					else if (strncmp("POST /", (*client).request, 6) == 0)
					{
						// post
						char *path = (*client).request + 4;
						char *end_path = strstr(path, " ");
						if (!end_path)
						{
							sendErrorPage(400, *client);
						}
						else
						{
							*end_path = 0;
							char *data = q + 4;
							// printf("recived data(%zu): |%s|\n", strlen(data), data);
							FILE *fp = fopen("cookies/1", "a");
							fwrite(data, strlen(data), 1, fp);
							fclose(fp);
							serve_resource_p(&(*client), path, data);
						}
					}
					else
					{
						sendErrorPage(400, *client);
					}
				}
			}
		}
		client++;
	}//while(client)
	std::cout << "1-end\n";
}

void getContentsList()
{

}

void getContent()
{

}

void getIndexPage()
{

}

void postContent()
{

}

void deleteContent()
{

}

void acceptSockets(std::vector<int> servers, fd_set reads)
{
	int server;
	for (int i = 0; i < servers.size(); i++)
	{
		server = servers[i];
		if (FD_ISSET(server, &reads))
		{
			makeClient();
			client_info &client = clients.back();
			client.socket = accept(server, (struct sockaddr*)&(client.address), &(client.address_length));
			std::cout << "client->socket: " << client.socket << "\n";
			if (client.socket < 0)
			{
				fprintf(stderr, "accept() failed. (%d)\n", errno);
				exit(1);
			}
			printf("New Connection from %s.\n", get_client_address(&client));
		}
	}
}

void handler(int signo)
{
	if (signo == SIGINT)
	{
		for (int i = 0; i < vec.size(); i++)
			close(vec[i]);
		std::cout << "close\n";
		exit(1);
	}
}

int main()
{
	signal(SIGINT, handler);
    //서버 소켓 생성
	int server = create_socket("127.0.0.1", "8080");
    int server2 = create_socket("127.0.0.1", "8081");
	vec.push_back(server);
	vec.push_back(server2);

	while (1)
	{
		fd_set reads;
        // 서버에 들어온 요청 확인
		// reads = wait_on_clients((int[]){server, server2});
		reads = wait_on_clients(vec);
		acceptSockets(vec, reads);
		std::cout << "1-start\n";
		sendResponse(reads);
	}
	printf("\nClosing socket...\n");
	close(server);
	close(server2);
	printf("Finished.\n");
	return 0;
}