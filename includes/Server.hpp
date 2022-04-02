#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include "DataStructs.hpp"
#include "Location.hpp"

class Server
{
public:
	int client_body_limit;
	bool autoindex;
	std::string root;
	std::string server_name;
	std::vector<std::string> index;
	std::vector<MethodType> allow_methods;
	std::vector<Location> locations;
	
	int redirect_status;
	std::string redirect_url;

public:
	std::string host;
	std::vector<std::string> port;
	
	std::vector<int> listen_socket;
	std::vector<int> fd;//??
	
	Server();
	~Server();

	void create_socket();
	void print_server_info();

	static MethodType s_to_methodtype(std::string str);
};

#endif