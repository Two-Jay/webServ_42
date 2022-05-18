#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
		return 0;

	std::cout << "=================================================" << std::endl;
	std::cout << "                 Webserv Start!                  " << std::endl;
	std::cout << "=================================================" << std::endl;

	std::string config = (argc == 1) ? "./config/default.config" : argv[1];
	ConfigParser configParser(config.c_str());
	std::vector<Server> *servers = configParser.parse();
	ServerManager manager(*servers);
	manager.print_servers_info();

	manager.create_servers();
	while (1)
	{
		manager.wait_on_clients();
		manager.accept_sockets();
		manager.treat_request();
	}
	std::cout << "> Closing socket..." << std::endl;
	manager.close_servers();
	
	std::cout << "=================================================" << std::endl;
	std::cout << "                Webserv Finished                 " << std::endl;
	std::cout << "=================================================" << std::endl;
	delete servers;
	return 0;
}