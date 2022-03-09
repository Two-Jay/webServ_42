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
	int get_body_size();

	void make_error_body();
	std::string make_header();
	std::string serialize();
};

#endif