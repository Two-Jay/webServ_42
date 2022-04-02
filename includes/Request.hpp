#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Utils.hpp"
#include "DataStructs.hpp"

class Request
{
public:
	std::string method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	Request();
	~Request();

	void parsing(std::string request);
	std::string get_path(); // url after program name and a slash
	std::string get_query(); // query string : after ?
};

#endif