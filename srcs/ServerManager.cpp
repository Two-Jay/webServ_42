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
					fprintf(stderr, "[ERROR] accept() failed. (%d)\n", errno);
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
		fprintf(stderr, "[ERROR] select() failed. (%d)\n", errno);
		exit(1);
	}
    //변화가 생긴 소켓
	this->reads = reads;
}

void ServerManager::drop_client(Client client)
{
	std::cout << "!! drop client !!\n";
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
	fprintf(stderr, "[ERROR] drop_client not found.\n");
	exit(1);
}

/*
** Response methods
*/

void ServerManager::send_response()
{
	for (int i = 0  ; i < clients.size() ; i++)
	{
		std::cout << "sendResponse-1\n";
		std::cout << "client.socket: " << clients[i].get_socket() << "\n";
		if (FD_ISSET(clients[i].get_socket(), &reads))
		{
			std::cout << "sendResponse-2\n";
			if (MAX_REQUEST_SIZE == clients[i].get_received_size())
			{
				send_error_page(400, clients[i]);
				continue;
			}
			
			// 최대 사이즈가 MAX 사이즈를 넘지 않게 받은 데이터 크기 체크
			// 이미 받은 데이터 다음위치를 체크해서 받음
			std::cout << "client.request" << ": " << clients[i].request << " / " << clients[i].get_received_size() << "\n";
			int r = recv(clients[i].get_socket(), 
					clients[i].request + clients[i].get_received_size(), 
					MAX_REQUEST_SIZE - clients[i].get_received_size(), 0);
			std::cout << "client.request" << ": " << clients[i].request << " / " << r << "\n";
			if (r < 1)
			{
				printf("Unexpected disconnect from (%d)%s.\n", r, clients[i].get_client_address());
				fprintf(stderr, "[ERROR] recv() failed. (%d)%s\n", errno, strerror(errno));
				if (errno == 2)
					send_error_page(404, clients[i]);
				drop_client(clients[i]);
				i--;
			}
			else
			{
				clients[i].set_received_size(clients[i].get_received_size() + r);
				clients[i].request[clients[i].get_received_size()] = 0;
				char *found = strstr(clients[i].request, "\r\n\r\n");
				if (found)
				{
					if (strncmp("GET /", clients[i].request, 5) == 0)
						get_method(clients[i]);
					else if (strncmp("POST /", clients[i].request, 6) == 0)
						post_method(clients[i]);
					else if (strncmp("DELETE /", clients[i].request, 8) == 0)
						delete_method(clients[i]);
					else
						send_error_page(400, clients[i]);
				}
			}
		}
	}
	std::cout << "1-end\n";
}

void ServerManager::send_error_page(int code, Client &client)
{
	std::cout << ">> send error page" << std::endl;
	Response response(status_info[code]);
	response.make_error_body();
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(response.get_body_size()));
	response.append_header("Content-Type", "text/html");

	std::string result = response.serialize();
	send(client.get_socket(), result.c_str(), result.size(), 0);
}

/*
** http methods
*/

void ServerManager::get_method(Client &client)
{
	std::cout << "GET method\n";
	char *path = client.request + 4;
	char *end_path = strstr(path, " ");
	if (!end_path)
	{
		send_error_page(400, client);
	}
	else
	{
		*end_path = 0;
		std::cout << path << std::endl;
		if (strlen(path) >= MAX_URI_SIZE)
		{
			send_error_page(400, client);
			return;
		}

		if (strcmp(path, "/") == 0) {
			// path = "/index.html";
			get_index_page(client);
			return ;
		}
		char *dir_list;
		std::string full_path = find_path_in_root(path, client);
		FILE *fp = fopen(full_path.c_str(), "rb");
		std::cout << ">> " + full_path + ", " + (fp == NULL ? "not found" : "found") << std::endl;
		if (!fp)
			send_error_page(404, client);
		else
		{
			fseek(fp, 0L, SEEK_END);
			size_t length = ftell(fp);
			rewind(fp);
			const char *type = find_content_type(full_path.c_str());

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
		}
		fclose(fp);
		drop_client(client);
	}
	std::cout << "pass" << std::endl;
}

void ServerManager::post_method(Client &client)
{
	std::cout << "POST method\n";
	// std::cout << "request: " << client.request << "\n";
	char *path = client.request + 4;
	char *end_path = strstr(path, " ");
	std::cout << "1\n";
	if (!end_path)
	{
		send_error_page(400, client);
	}
	else
	{
		// *end_path = 0;
		// std::string body = strstr(client.request, "\r\n\r\n") + 4;
		std::cout << "request: " << client.request << "\n";
		char * body = strstr(client.request, "\r\n\r\n") + 4;
		std::cout << "body: " << body << "\n";
		// replace(body, "+", " ");
		// char *title, *content;
		std::string title, content;
		title = (std::string)(strstr(body, "title=") + 6);
		for (int i = 0; i < title.size(); i++) {
			if (title[i] == '&') {
				title[i] = '\0';
			}
		}
		content = (std::string)(strstr(body, "content=") + 8);
		std::cout << "title: " << title << ", content: " << content << "\n";
		// title=test+title%2B&content=description+test%2Btest
		// 
		// server 뒤져서 location 뒤져서.. body에 맞는 path와 method가 존재하는지 찾고..
		// 그 찾은 부분의 root를 저장하는 경로로...... 
		// printf("recived data(%zu): |%s|\n", strlen(data), data);
		// FILE *fp = fopen("data/" + title, "w");
		fp = fopen(("www/html/data/" + title).c_str(), "w");
		fwrite(content.c_str(), content.size(), 1, fp);
		fclose(fp);
		// free(title);
		// free(content);
		// post_content();
	}
}

void ServerManager::delete_method(Client &client)
{
	std::cout << "DELETE method\n";
}

std::string ServerManager::get_contents_list()
{
	std::string path = "www/html/data/";
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path.c_str());

	std::string result = "<ul>";
	while ((ent = readdir(dir)) != NULL)
	{
		if ((std::string)ent->d_name == "." || (std::string)ent->d_name == "..")
			continue;
		result += "<li><a href=\"/data/" + (std::string)ent->d_name + "\">" 
			+ (std::string)ent->d_name + "</a></li>";
	}
	result += "</ul>";
	return result;
}

void ServerManager::get_content()
{

}

void ServerManager::get_index_page(Client &client)
{
	std::string list;
	std::string result = "<!DOCTYPE html>"
"<html>"
	"<head>"
		"<meta charset=\"UTF-8\" />"
		"<title>webserv</title>"
	"</head>"
	"<body>"
		"<h1>42 webserv</h1>";
	result += get_contents_list();
	result += "</body></html>";
	Response response(status_info[200]);
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(result.length()));
	response.append_header("Content-Type", "text/html");
	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);
	send(client.get_socket(), result.c_str(), result.length(), 0);
	drop_client(client);
}

void ServerManager::post_content()
{
}

void ServerManager::delete_content()
{

}

/*
** helper methods
*/

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

std::string ServerManager::find_path_in_root(const char *path, Client &client)
{
	// server의 location의 root파악해서 가져오기
	std::string full_path;
	full_path.append("www/html");
	full_path.append(path);
	return full_path;
}