#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(std::vector<Server> servers)
{
	this->servers = servers;

	status_info.insert(std::make_pair(200, "200 OK"));
	status_info.insert(std::make_pair(201, "201 Created"));
	status_info.insert(std::make_pair(204, "204 No Content"));
	status_info.insert(std::make_pair(300, "300 Multiple Choices"));
	status_info.insert(std::make_pair(301, "301 Moved Permanently"));
	status_info.insert(std::make_pair(302, "302 Found"));
	status_info.insert(std::make_pair(303, "303 See Other"));
	status_info.insert(std::make_pair(307, "307 Temporary Redirect"));
	status_info.insert(std::make_pair(400, "400 Bad Request"));
	status_info.insert(std::make_pair(401, "401 Unauthorized"));
	status_info.insert(std::make_pair(403, "403 Forbidden"));
	status_info.insert(std::make_pair(404, "404 Not found"));
	status_info.insert(std::make_pair(405, "405 Method Not Allowed"));
	status_info.insert(std::make_pair(408, "408 Request Timeout"));
	status_info.insert(std::make_pair(411, "411 Length Required"));
	status_info.insert(std::make_pair(413, "413 Request Entity Too Large"));
	status_info.insert(std::make_pair(414, "414 URI Too Long"));
	status_info.insert(std::make_pair(429, "429 Too Many Request"));
	status_info.insert(std::make_pair(500, "500 Internal Server Error"));
	status_info.insert(std::make_pair(502, "502 Bad Gateway"));
	status_info.insert(std::make_pair(504, "504 Gateway Timeout"));
	status_info.insert(std::make_pair(505, "505 HTTP Version Not Supported"));
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
	int recv;
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
		if (errno == EINVAL) 
		{
			for (int i = 0; i < clients.size(); i++)
				send_error_page(429, clients[i]);
		}
		else 
		{
			for (int i = 0; i < clients.size(); i++)
				send_error_page(500, clients[i]);
		}
		exit(1);
	}
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

bool ServerManager::handle_CGI(Request *request, Location *loc)
{
	for (std::map<std::string, std::string>::iterator it = loc->cgi_info.begin();
	it != loc->cgi_info.end(); it++)
	{
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
				drop_client(clients[i]);
				continue;
			}
			int r = recv(clients[i].get_socket(), 
					clients[i].request + clients[i].get_received_size(), 
					MAX_REQUEST_SIZE - clients[i].get_received_size(), 0);
			clients[i].set_received_size(clients[i].get_received_size() + r);
			int recv_size = clients[i].get_received_size();
			if (r < 1)
			{
				std::cout << "> Unexpected disconnect from (" << r << ")[" << clients[i].get_client_address() << "]." << std::endl;
				fprintf(stderr, "[ERROR] recv() failed. (%d)%s\n", errno, strerror(errno));
				if (errno == 2)
					send_error_page(404, clients[i]);
				send_error_page(500, clients[i]);
				drop_client(clients[i]);
				i--;
			}
			else if (clients[i].request[recv_size - 4] == '\r' && clients[i].request[recv_size - 3] == '\n'
				&& clients[i].request[recv_size - 2] == '\r' && clients[i].request[recv_size - 1] == '\n')
			{
				std::cout << "199: " << clients[i].request << "\n";
				Request req = Request(clients[i].get_socket());
				int error_code;
				if ((error_code = req.parsing(clients[i].request)))
				{
					send_error_page(error_code, clients[i]);
					drop_client(clients[i]);
					continue;
				}
				
				if (req.headers.find("Content-Length") != req.headers.end() && 
				stoi(req.headers["Content-Length"]) > clients[i].server->client_body_limit)
				{
					send_error_page(413, clients[i]);
					drop_client(clients[i]);
					continue;
				}

				// clients[i].set_received_size(clients[i].get_received_size() + r);
				clients[i].request[clients[i].get_received_size()] = 0;

				Location* loc = clients[i].server->get_cur_location(req.get_path());
				std::vector<MethodType> method_list;
				loc ? method_list = loc->allow_methods : method_list = clients[i].server->allow_methods;
				if (!is_allowed_method(method_list, req.method))
				{
					send_405_error_page(405, clients[i], method_list);
					drop_client(clients[i]);
					continue;
				}

				std::cout << "Request: " << req;
				if (loc && handle_CGI(&req, loc))
				{
					CgiHandler cgi(req);
					cgi.cgi_exec(req, *loc);
					return ;
				}
				// body size 검사 해야함
				// 클라이언트 바디 리미트 넘어가면 413번 넘어가야함
				// Content_length 체크해서.
				if (is_response_timeout(clients[i]) == true)
					send_error_page(408, clients[i]);
				if (clients[i].server->redirect_status != -1)
					send_redirection(clients[i], req.method);
				else if (req.method == "GET")
					get_method(clients[i], req.path);
				else if (req.method == "POST")
					post_method(clients[i], req);
				else if (req.method == "DELETE")
					delete_method(clients[i], req.path);
				drop_client(clients[i]);
			}
		}
	}
}

bool ServerManager::is_response_timeout(Client& client) {
	static timeval tv;
	
	gettimeofday(&tv, NULL);
	if (tv.tv_sec - client.get_last_time().tv_sec > client.server->recv_timeout.tv_sec) return true;
	client.set_last_time_sec(tv);
	return false;
}

void ServerManager::send_redirection(Client &client, std::string request_method)
{
	std::cout << ">> send redirection response" << std::endl;
	Response response(status_info[client.server->redirect_status]);
	if (client.server->redirect_status == 300)
		response.make_status_body(client.server->redirect_url);
	else
		response.make_status_body();
	response.append_header("Server", client.server->server_name);
	response.append_header("Date", get_current_date_GMT());
	response.append_header("Content-Type", "text/html");
	response.append_header("Content-Length", std::to_string(response.get_body_size()));
	response.append_header("Connection", "keep-alive");
	response.append_header("Location", client.server->redirect_url);

	std::string result = response.serialize();
	send(client.get_socket(), result.c_str(), result.size(), 0);
}

void ServerManager::send_error_page(int code, Client &client)
{
	std::cout << ">> send error page" << std::endl;
	Response response(status_info[code]);
	response.make_status_body();
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(response.get_body_size()));
	response.append_header("Content-Type", "text/html");

	std::string result = response.serialize();
	send(client.get_socket(), result.c_str(), result.size(), 0);
}

void ServerManager::send_405_error_page(int code, Client &client, std::vector<MethodType> allow_methods)
{
	std::cout << ">> send error page" << std::endl;
	std::string allowed_method_list;
	Response response(status_info[code]);
	response.make_status_body();
	response.append_header("Connection", "close");
	response.append_header("Content-Length", std::to_string(response.get_body_size()));
	response.append_header("Content-Type", "text/html");
	for (int i = 0; i < allow_methods.size(); i++)
	{
		allowed_method_list += methodtype_to_s(allow_methods[i]);
		if (i < allow_methods.size() - 1)
			allowed_method_list += ", ";
	}
	response.append_header("Allow", allowed_method_list);

	std::string result = response.serialize();
	send(client.get_socket(), result.c_str(), result.size(), 0);
}

int	ServerManager::is_allowed_method(std::vector<MethodType> allow_methods, std::string method) 
{
	if (method == "GET")
		return true;
	for (std::vector<MethodType>::iterator it = allow_methods.begin(); 
	it != allow_methods.end(); it++)
	{
		if (method == methodtype_to_s(*it))
			return true;
	}
	return false;
}

std::string ServerManager::methodtype_to_s(MethodType method) {
	if (method == GET)
		return "GET";
	else if (method == POST)
		return "POST";
	else if (method == DELETE)
		return "DELETE";
	return "";
}

/*
** http methods
*/

bool is_loc_check(std::string path, Client &client)
{
	Location *cur_loc = client.server->get_cur_location(path);
	if (!cur_loc)
		return false;
	std::string root = cur_loc->path;
	std::cout << "loc_check\n";
	std::cout << "path: " << path << ", root: " << root << "\n";
	if (path == root)
	{
		// path.append("/" + root);
		return true;
	}
	return false;
}

void ServerManager::get_method(Client &client, std::string path)
{
	std::cout << "GET method\n";
	std::cout << "path: " << path << "\n";

	if (path.length() >= MAX_URI_SIZE)
	{
		send_error_page(414, client);
		return;
	}

	if (path == "/")
	{
		int flag = false;
		// index page 중에 하나
		std::string root = client.get_root_path(path);
		for (int i = 0; i < client.server->index.size(); i++)
		{
			FILE *fp = fopen((root + "/" + client.server->index[i]).c_str(), "rb");
			if (fp)
			{
				flag = true;
				fclose(fp);
				path = "/" + client.server->index[i];
				break;
			}
		}
		if (client.server->autoindex)
			flag = true;
		if (!flag)
		{
			send_error_page(403, client);
			return;
		}
	}
	else if (is_loc_check(path, client))
	{
		std::vector<std::string> loc = client.server->get_cur_location(path)->index;
		for (int i = 0; i < loc.size();i++)
		{
			FILE *fp = fopen((client.server->get_cur_location(path)->root + "/" + loc[i]).c_str(), "rb");
			if (fp)
			{
				fclose(fp);
				path = "/" + loc[i];
				break;
			}
		}
	}
	// std::cout << "path: " << path << "\n";

	char *dir_list;
	struct stat buf;
	std::string full_path = find_path_in_root(path, client);
	lstat(full_path.c_str(), &buf);
	FILE *fp = fopen(full_path.c_str(), "rb");
	std::cout << ">> " + full_path + ", " + (fp == NULL ? "not found" : "found") << std::endl;
	if (!fp)
		send_error_page(404, client);
	else
	{
		if (full_path.back() == '/' && client.server->autoindex)
			get_autoindex_page(client, path);
		else if (S_ISDIR(buf.st_mode))
		{
			send_error_page(404, client);
			return;
		}
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

void ServerManager::post_method(Client &client, Request &request)
{
	std::cout << "POST method\n";

	if (request.headers.find("Content-Length") == request.headers.end())
	{
		send_error_page(411, client);
		return;
	}

	std::string full_path = find_path_in_root(request.path, client);
	size_t index = full_path.find_last_of("/");
	if (index == std::string::npos)
	{
		send_error_page(500, client);
		return;
	}
	std::string file_name = full_path.substr(index + 1);
	std::string folder_path = full_path.substr(0, index);

	std::string command = "mkdir -p " + folder_path;
	system(command.c_str());
	FILE *fp = fopen(full_path.c_str(), "w");
	if (!fp)
	{
		send_error_page(500, client);
		return;
	}

	fwrite(request.body.c_str(), request.body.size(), 1, fp);
	fclose(fp);
	
	Response response(status_info[201]);
	response.append_header("Connection", "close");
	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);
}

void ServerManager::delete_method(Client &client, std::string path)
{
	std::cout << "DELETE method\n";
	std::string full_path = find_path_in_root(path, client);
	std::cout << full_path << std::endl;

	std::cout << full_path << std::endl;
	FILE *fp = fopen(full_path.c_str(), "r");
	if (!fp)
	{
		send_error_page(204, client);
		return;
	}
	fclose(fp);

	std::remove(full_path.c_str());
	Response response(status_info[200]);
	response.append_header("Connection", "close");

	std::string header = response.make_header();
	send(client.get_socket(), header.c_str(), header.size(), 0);
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
	std::string full_path = "";
	std::string location;
	full_path.append(client.get_root_path(path));
	Location *loc = client.server->get_cur_location(path);
	if (loc)
		location = loc->path;
	else
		location = "";
	std::string str = path.substr(location.length());
	full_path.append(str);
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