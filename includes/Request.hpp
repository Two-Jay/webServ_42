#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include "Utils.hpp"
#include "DataStructs.hpp"

class Request
{
private:
	// bool chunked;
	// int content_length;
	// std::string content_type;
	// std::string host;
	// std::string body;
	// std::string path;
	// MethodType method;
	// last time -> manager에
	

public:
	std::map<std::string, std::string> headers;
	std::string body;
	Request();
	~Request();

	void parsing(std::string request);
	std::string getPath(); // url after program name and a slash
	std::string getQuery(); // query string : after ?
};

#endif