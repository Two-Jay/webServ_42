#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <list>
#include <iostream>
#include <vector>
#include <signal.h>
#include <string>

#define MAX_REQUEST_SIZE 2047
#define BSIZE 1024

struct client_info {
	socklen_t address_length;
	struct sockaddr_storage address;
	int socket;
	char request[MAX_REQUEST_SIZE + 1];
	int received;
	// struct client_info *next;
};

// static struct client_info *clients = 0;
static std::list<client_info> clients;
std::vector<int> vec;