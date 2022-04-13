#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(std::vector<Server> servers)
{
	this->servers = servers;

	status_info.insert(std::make_pair(200, "200 OK"));
	status_info.insert(std::make_pair(201, "201 Created"));
	status_info.insert(std::make_pair(204, "204 No Content"));
	status_info.insert(std::make_pair(301, "301 Moved Permanently"));
	status_info.insert(std::make_pair(302, "302 Found"));
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
				clients.push_back(Client(&servers[i]));
				Client &client = clients.back();
				client.set_socket(accept(server, (struct sockaddr*)&(client.address), &(client.address_length)));
				if (client.get_socket() < 0)
				{
					fprintf(stderr, "[ERROR] accept() failed. (%d)\n", errno);
					exit(1);
				}
				std::cout << "> New Connection from [" << client.get_client_address() << "]." << std::endl;
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

void ServerManager::print_servers_info()
{
	std::cout << std::endl;
	std::cout << "=================================================" << std::endl;
	std::cout << "            Total Server Informations            " << std::endl;
	std::cout << "=================================================" << std::endl;
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].print_server_info();
	}
	std::cout << "=================================================" << std::endl;
	std::cout << std::endl;
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

bool handleCGI(Request *request, Location *loc) {
	for (std::map<std::string, std::string>::iterator it = loc->cgi_info.begin();
	it != loc->cgi_info.end(); it++) {
		if (request->get_path().find(it->first) != std::string::npos)
			return true;
	}
	return false;
}

void ServerManager::treat_request()
{
	for (int i = 0  ; i < clients.size() ; i++)
	{
		if (FD_ISSET(clients[i].get_socket(), &reads))
		{
			if (MAX_REQUEST_SIZE == clients[i].get_received_size())
			{
				send_error_page(400, clients[i]);
				continue;
			}
			
			int r = recv(clients[i].get_socket(), 
					clients[i].request + clients[i].get_received_size(), 
					MAX_REQUEST_SIZE - clients[i].get_received_size(), 0);
			if (r < 1)
			{
				std::cout << "> Unexpected disconnect from (" << r << ")[" << clients[i].get_client_address() << "]." << std::endl;
				fprintf(stderr, "[ERROR] recv() failed. (%d)%s\n", errno, strerror(errno));
				if (errno == 2)
					send_error_page(404, clients[i]);
				drop_client(clients[i]);
				i--;
			}
			else
			{
				Request req = Request(clients[i].get_socket());
				req.parsing(clients[i].request);
				
				clients[i].set_received_size(clients[i].get_received_size() + r);
				clients[i].request[clients[i].get_received_size()] = 0;

				Location* loc = clients[i].server->currLocation(req.get_path());
				if (handleCGI(&req, loc)) {
					CgiHandler cgi(req);
					cgi.cgi_exec(req, *loc);
					return ;
				}
				// body size 검사 해야함
				if (req.method == "GET")
					get_method(clients[i], req.path);
				else if (req.method == "POST")
					post_method(clients[i], req);
				else if (req.method == "DELETE")
					delete_method(clients[i], req.path);
				else
					send_error_page(400, clients[i]);
				drop_client(clients[i]);
			}
		}
	}
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

void ServerManager::get_method(Client &client, std::string path)
{
	std::cout << "GET method\n";

	if (path.length() >= MAX_URI_SIZE)
	{
		send_error_page(400, client);
		return;
	}

	if (path == "/")
	{
		// index page 중에 하나
		std::string root = client.get_root_path(path);
		for (int i = 0; i < client.server->index.size(); i++)
		{
			FILE *fp = fopen((root + "/" + client.server->index[i]).c_str(), "rb");
			if (fp)
			{
				fclose(fp);
				path = "/" + client.server->index[i];
				break;
			}
		}
	}

	if (path == "/board") path = "/board.html";
	if (path == "/board/content") get_board_content(client);
	else
	{
		char *dir_list;
		std::string full_path = find_path_in_root(path, client);
		FILE *fp = fopen(full_path.c_str(), "rb");
		std::cout << ">> " + full_path + ", " + (fp == NULL ? "not found" : "found") << std::endl;
		if (!fp) send_error_page(404, client);
		else
		{
			if (full_path.back() == '/' && client.server->autoindex)
				get_autoindex_page(client, path);
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
		}
		fclose(fp);
	}
}

void ServerManager::post_method(Client &client, Request &request)
{
	std::cout << "POST method\n";

	std::string title, content;
	int start = request.body.find("title=") + 6;
	int end = request.body.find("&", start);
	title = request.body.substr(start, end - start);
	start = request.body.find("content=", end) + 8;
	content = request.body.substr(start, request.body.length() - start);

	std::string root_path = client.get_root_path(request.path);
	FILE *fp = fopen((root_path + "/" + title).c_str(), "w");
	if (!fp)
	{
		system(("mkdir -p " + client.get_root_path(request.path)).c_str());
		fp = fopen((root_path + "/" + title).c_str(), "w");
	}
	if (!fp)
	{
		send_error_page(500, client);
		return;
	}
	fwrite(content.c_str(), content.size(), 1, fp);
	fclose(fp);
	
	if (request.path == "/board/content")
	{
		Response response(status_info[302]);
		response.append_header("Location", request.headers["Referer"]);
		std::string header = response.make_header();
		send(client.get_socket(), header.c_str(), header.size(), 0);
	}
	else
	{
		Response response(status_info[201]);
		response.append_header("Connection", "close");
		std::string header = response.make_header();
		send(client.get_socket(), header.c_str(), header.size(), 0);
	}
}

void ServerManager::delete_method(Client &client, std::string path)
{
	std::cout << "DELETE method\n";
	std::string full_path = find_path_in_root(path, client);
	std::cout << full_path << std::endl;
	// std::remove(full_path.c_str());

	std::cout << full_path << std::endl;
	FILE *fp = fopen(full_path.c_str(), "r");
	if (!fp)
	{
		send_error_page(204, client);
		return;
	}
	fclose(fp);
	
	Response response(status_info[200]);
	response.append_header("Connection", "close");

	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);
}

void ServerManager::get_board_content(Client &client)
{
	std::string list;
	std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" /><title>webserv</title></head><body><h1>webserv</h1>";
	result += make_content_list();
	result += "</body></html>";

	Response response(status_info[200]);
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(result.length()));
	response.append_header("Content-Type", "text/html");

	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);
	send(client.get_socket(), result.c_str(), result.length(), 0);
}

std::string ServerManager::make_content_list()
{
	std::string path = "www/html/contents/";
	DIR *dir;
	struct dirent *ent;
	dir = opendir("www/html/contents/");

	std::string result = "<ul>";
	while ((ent = readdir(dir)) != NULL)
	{
		if ((std::string)ent->d_name == "." || (std::string)ent->d_name == "..")
			continue;
		result += "<li><a href=\"/contents/" + (std::string)ent->d_name + "\">" 
			+ (std::string)ent->d_name + "</a></li>";
	}
	result += "</ul>";
	return result;
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

std::string ServerManager::find_path_in_root(std::string path, Client &client)
{
	std::string full_path;
	full_path.append(client.get_root_path(path));
	full_path.append(path);
	return full_path;
}

void ServerManager::get_autoindex_page(Client &client, std::string path)
{
	std::string addr;
	std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
		"<title>webserv</title></head><body><h1>webserv</h1><h2>Index of ";
	result += path;
	result += "<hr><div>";

	DIR *dir = NULL;
	if ((dir = opendir((client.get_root_path(path) + path).c_str())) == NULL)
		return;

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL)
	{
		result += "<a href=\"" + addr + file->d_name;
		result += (file->d_type == DT_DIR ? "/" : "") + (std::string)"\">";
		result += (std::string)(file->d_name) + (file->d_type == DT_DIR ? "/" : "") + "</a><br>";
	}
	closedir(dir);

	result += "</div></body></html>";

	Response response(status_info[200]);
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(result.length()));
	response.append_header("Content-Type", "text/html");
	std::string header = response.make_header();
	
	send(client.get_socket(), header.c_str(), header.size(), 0);
	send(client.get_socket(), result.c_str(), result.length(), 0);
}