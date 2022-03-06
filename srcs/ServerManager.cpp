#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(std::vector<Server> servers)
{
	this->servers = servers;

	status_info.insert(std::make_pair(200, "200 OK"));
	status_info.insert(std::make_pair(201, "201 Created"));
	status_info.insert(std::make_pair(204, "204 No Content"));
	status_info.insert(std::make_pair(301, "301 Moved Permanently"));
	status_info.insert(std::make_pair(307, "307 Temporary Redirect"));
	status_info.insert(std::make_pair(400, "400 Bad Request"));
	status_info.insert(std::make_pair(401, "401 Unauthorized"));
	status_info.insert(std::make_pair(403, "403 Forbidden"));
	status_info.insert(std::make_pair(404, "404 Not found"));
	status_info.insert(std::make_pair(405, "405 Method Not Allowed"));
	status_info.insert(std::make_pair(408, "408 Request Timeout"));
	status_info.insert(std::make_pair(414, "414 URI Too Long"));
	status_info.insert(std::make_pair(429, "429 Too Many Request"));
	status_info.insert(std::make_pair(500, "500 Internal Server Error"));
	status_info.insert(std::make_pair(502, "502 Bad Gateway"));
	status_info.insert(std::make_pair(504, "504 Gateway Timeout"));
}

ServerManager::~ServerManager()
{
}

/*
** Server methods
*/

void ServerManager::create_servers()
{
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].create_socket();
	}
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
				clients.push_back(Client());
				Client &client = clients.back();
				client.set_socket(accept(server, (struct sockaddr*)&(client.address), &(client.address_length)));
				std::cout << "client->socket: " << client.get_socket() << "\n";
				if (client.get_socket() < 0)
				{
					fprintf(stderr, "accept() failed. (%d)\n", errno);
					exit(1);
				}
				printf("New Connection from %s.\n", client.get_client_address());
			}
		}
	}
}

void ServerManager::close_servers()
{
	for (int i = 0; i < servers.size(); i++)
	{
		for (int j = 0; j < servers[i].listen_socket.size(); j++)
			close(servers[i].listen_socket[j]);
	}
}

/*
** Client methods
*/

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
		FD_SET(clients[i].get_socket(), &reads);
		if (clients[i].get_socket() > max)
			max = clients[i].get_socket();
	}

	if (select(max + 1, &reads, 0, 0, 0) < 0)
	{
		fprintf(stderr, "select() failed. (%d)\n", errno);
		exit(1);
	}
    //변화가 생긴 소켓
	this->reads = reads;
}

void ServerManager::drop_client(Client client)
{
	close(client.get_socket());

	std::vector<Client>::iterator iter;
	for (iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter).get_socket() == client.get_socket())
		{
			clients.erase(iter);
			return;
		}
	}
	fprintf(stderr, "drop_client not found.\n");
	exit(1);
}

/*
** Response methods
*/

void ServerManager::send_response()
{
	std::vector<Client>::iterator client = clients.begin();
	while (client != clients.end())
	{
		std::cout << "sendResponse-1\n";
		std::cout << "client.socket: " << client->get_socket() << "\n";
		if (FD_ISSET(client->get_socket(), &reads))
		{
			std::cout << "sendResponse-2\n";
			if (MAX_REQUEST_SIZE == client->get_received_size())
			{
				send_error_page(400, *client);
				continue;
			}
			
			// 받은 데이터 크기 체크
			// 이미 받은 데이터 다음위치를 체크해서 받음
			// 최대 사이즈가 MAX 사이즈를 넘지 않게
			std::cout << "client.request" << ": " << (*client).request << " / " << (*client).get_received_size() << "\n";
			int r = recv((*client).get_socket(), 
					(*client).request + (*client).get_received_size(), 
					MAX_REQUEST_SIZE - (*client).get_received_size(), 0);
			if (r < 1)
			{
				printf("Unexpected disconnect from (%d)%s.\n", r, (*client).get_client_address());
				fprintf(stderr, "recv() failed. (%d)\n", errno);
				fprintf(stderr, "%s\n", strerror(errno));
				drop_client(*client);
			}
			else
			{
				(*client).set_received_size((*client).get_received_size() + r);
				(*client).request[(*client).get_received_size()] = 0;
				char *found = strstr((*client).request, "\r\n\r\n");
				if (found)
				{
					if (strncmp("GET /", (*client).request, 5) == 0)
						get_method(*client);
					else if (strncmp("POST /", (*client).request, 6) == 0)
						post_method(*client);
					else if (strncmp("DELETE /", (*client).request, 8) == 0)
						delete_method(*client);
					else
						send_error_page(400, *client);
				}
			}
		}
		client++;
	}
	std::cout << "1-end\n";
}

void ServerManager::send_error_page(int code, Client &client)
{
	const char *response = Response(status_info[code]).make_error_page().c_str();
	send(client.get_socket(), response, strlen(response), 0);
}

const char *ServerManager::find_content_type(const char *path)
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

/*
** http methods
*/

void ServerManager::get_method(Client &client)
{
	std::cout << "GET /\n";
	char *path = client.request + 4;
	std::cout << path << std::endl;
	char *end_path = strstr(path, " ");
	if (!end_path)
	{
		send_error_page(400, client);
	}
	else
	{
		*end_path = 0;
		get_index_page(client, path);
		// get_content() get_contents_list() get_index_page() 셋 중 하나
	}
	std::cout << "pass" << std::endl;
}

void ServerManager::post_method(Client &client)
{
	// post
	char *path = client.request + 4;
	char *end_path = strstr(path, " ");
	if (!end_path)
	{
		send_error_page(400, client);
	}
	else
	{
		*end_path = 0;
		char *body = strstr(client.request, "\r\n\r\n") + 4;
		// server 뒤져서 location 뒤져서.. body에 맞는 path와 method가 존재하는지 찾고..
		// 그 찾은 부분의 root를 저장하는 경로로...... 
		// printf("recived data(%zu): |%s|\n", strlen(data), data);
		FILE *fp = fopen("cookies/1", "a");
		fwrite(body, strlen(body), 1, fp);
		fclose(fp);
		post_content();
	}
}

void ServerManager::delete_method(Client &client)
{

}

void ServerManager::get_contents_list()
{

}

void ServerManager::get_content()
{

}

void ServerManager::get_index_page(Client &client, const char *path)
{
	if (strcmp(path, "/") == 0) path = "index.html";
	if (strlen(path) > 100)
	{
		send_error_page(400, client);
		return;
	}
	if (strstr(path, ".."))
	{
		send_error_page(404, client);
		return ;
	}
	
	char full_path[128];
	sprintf(full_path, "www/html/%s", path);
	FILE *fp = fopen(full_path, "rb");
	if (!fp)
	{
		send_error_page(404, client);
		return ;
	}
	fseek(fp, 0L, SEEK_END);
	size_t length = ftell(fp);
	rewind(fp);
	const char *type = find_content_type(full_path);

	Response response(status_info[200]);
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(length));
	response.append_header("Content-Type", type);

	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);

	char buffer[BSIZE];
	int r = fread(buffer, 1, BSIZE, fp);
	while (r)
	{
		send(client.get_socket(), buffer, r, 0);
		r = fread(buffer, 1, BSIZE, fp);
	}
	fclose(fp);
	std::cout << "drop client\n";
	drop_client(client);
}

void ServerManager::post_content()
{
}

void ServerManager::delete_content()
{

}
