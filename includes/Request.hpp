#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include "MethodType.hpp"

class Request
{
private:
	bool chunked;
	int content_length;
	std::string content_type;
	std::string host;
	std::string body;
	MethodType method;
	// last time -> manager에

public:
	Request();
	~Request();
};

#endif