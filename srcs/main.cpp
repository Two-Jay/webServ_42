#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"

void print_webserv_welcome()
{
	std::cout << "=================================================" << std::endl;
	std::cout << "                 Webserv Start!                  " << std::endl;
	std::cout << "=================================================" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc > 2)
		return 0;

	print_webserv_welcome();

	std::string config = (argc == 1) ? "./config/default.config" : argv[1];
	ConfigParser configParser(config.c_str());
	std::vector<Server> *servers = configParser.parse();
	ServerManager manager(*servers);
	// manager.print_servers_info();

	// std::vector<Server> vec;
	// Server server;
	// server.host = "127.0.0.1";
	// server.port.push_back("8080");
	// server.port.push_back("8081");
	// vec.push_back(server);
	// ServerManager manager(vec);

    // //서버 소켓 생성
	// manager.create_servers();

	// while (1)
	// {
    //     // 서버에 들어온 요청 확인
	// 	manager.wait_on_clients();
	// 	manager.accept_sockets();
	// 	std::cout << "1-start\n";
	// 	manager.send_response();
	// }
	// printf("\nClosing socket...\n");
	// manager.close_servers();
	// printf("Finished.\n");
	delete servers;
	return 0;
}