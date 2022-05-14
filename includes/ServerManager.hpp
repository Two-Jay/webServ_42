#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <cstdio>
#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CgiHandler.hpp"

class ServerManager
{
private:
	std::vector<Server> servers;
	std::vector<Client> clients;
	int max_fd;
	fd_set reads;

	std::map<int, std::string> status_info;

	ServerManager();

public:
	ServerManager(std::vector<Server> servers);
	~ServerManager();

	void accept_sockets();
	void wait_on_clients();
	void drop_client(Client client);

	void create_servers();
	void close_servers();

	void treat_request();
	void print_servers_info();

private:
	void send_error_page(int code, Client &Client);
	bool handle_CGI(Request *request, Location *loc);
	bool is_response_timeout(Client& client);

	void get_method(Client &client, std::string path);
	void post_method(Client &client, Request &request);
	void delete_method(Client &client, std::string path);

	void get_autoindex_page(Client &client, std::string path);

	const char *find_content_type(const char *path);
	std::string find_path_in_root(std::string path, Client &client);
};

#endif