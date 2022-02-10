#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
private:
	std::string status_code;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	Response(/* args */);
	~Response();

	std::string serialize();
};

#endif