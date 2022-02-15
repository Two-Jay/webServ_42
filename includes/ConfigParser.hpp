#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <fstream>
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