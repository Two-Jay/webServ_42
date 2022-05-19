#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
private:
	std::map<std::string, std::string> headers;
	std::string body;

	Response();

public:
	std::string status_code;
	std::string status_phrase;
	Response(std::string status);
	~Response();

	void append_header(std::string first, std::string second);
	int get_body_size();

	void make_status_body();
	void make_status_body(std::string url);
	std::string make_header();
	std::string serialize();
};

#endif