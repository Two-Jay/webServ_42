#include "../includes/ServerManager.hpp"
#include "../includes/ConfigParser.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
		return 0;
	
	ConfigParser configParser(argv[1]);
	configParser.parse();
	return 0;
}