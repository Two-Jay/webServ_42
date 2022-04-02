#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include "Utils.hpp"
#include "DataStructs.hpp"

class Request
{
public:
	std::map<std::string, std::string> headers;
	std::string body;
	Request();
	~Request();

	void parsing(std::string request);
	std::string get_path(); // url after program name and a slash
	std::string get_query(); // query string : after ?
};

#endif