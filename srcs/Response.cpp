#include "../includes/Response.hpp"

Response::Response(std::string status)
{
	this->status = status;
}

Response::~Response()
{
}

void Response::append_header(std::string first, std::string second)
{
	headers.insert(std::make_pair(first, second));
}

int Response::get_body_size()
{
	return body.size();
}

std::string Response::make_header()
{
	std::string result;

	result.append("HTTP/1.1 " + status + "\r\n");
	for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
	{
		result.append((*i).first + ": " + (*i).second + "\r\n");	
	}
	result.append("\r\n");

	return result;
}

void Response::make_error_body()
{
	std::string result;

	result.append("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"/><title>webserv</title></head>");
	result.append("<body>");
	result.append("<h1>" + status.substr(0, 3) + "</h1>");
	result.append("<h3>" + status.substr(4, status.size()) + "</h3>");
	// index.html을 index 목록의 페이지 명으로 변경해야하는 것이 아닌가,.?
	result.append("<p>Click <a href=\"/index.html\">here</a> to return home.</p>");
	result.append("</body></html>");
	
	body.clear();
	body = result;
}

std::string Response::serialize()
{
	std::string result;

	result.append("HTTP/1.1 " + status + "\r\n");
	for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
	{
		result.append((*i).first + ": " + (*i).second + "\r\n");	
	}
	result.append("\r\n");
	result.append(body);

	return result;
}