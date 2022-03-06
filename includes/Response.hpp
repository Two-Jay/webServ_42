#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
private:
	std::string status;
	std::map<std::string, std::string> headers;
	std::string body;

	Response();

public:
	Response(std::string status);
	~Response();

	void append_header(std::string first, std::string second);

	std::string make_header();
	std::string make_error_page();
};

#endif