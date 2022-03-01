#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"


void handler(int signo)
{
	if (signo == SIGINT || signo == SIGQUIT)
	{
		for (int i = 0; i < vec.size(); i++)
			close(vec[i]);
		std::cout << "close\n";
		exit(1);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return 0;
	
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);

	ConfigParser configParser(argv[1]);
	ServerManager manager(configParser.parse());

    //서버 소켓 생성
	manager.create_servers();

	while (1)
	{
		fd_set reads;
        // 서버에 들어온 요청 확인
		// reads = wait_on_clients((int[]){server, server2});
		manager.wait_on_clients();
		manager.accept_sockets();
		std::cout << "1-start\n";
		sendResponse(reads);
	}
	printf("\nClosing socket...\n");
	close(server);
	close(server2);
	printf("Finished.\n");
	return 0;
}