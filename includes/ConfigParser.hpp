#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Server.hpp"

class ConfigParser
{
private:
	std::string content;
	
public:
	ConfigParser(const char* filename);
	~ConfigParser();

	std::vector<Server> parse();
};

#endif