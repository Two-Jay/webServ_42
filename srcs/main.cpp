#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
		return 0;

	std::cout << argv[0] << std::endl;

	ConfigParser configParser(argv[1]);
	ServerManager manager(configParser.parse());

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
	return 0;
}