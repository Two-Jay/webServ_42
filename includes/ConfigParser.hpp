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
	Server parse_server(size_t *i);
	Location parse_location(size_t *i);

	int print_parse_error();
};

#endif