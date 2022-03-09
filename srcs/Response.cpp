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

std::string Response::make_error_page()
{
	std::string result;

	result.append("<!DOCTYPE html> <html> <head> <meta charset=\"UTF-8\"/> <title>webserv</title> </head>");
	result.append("<body>");
	result.append("<h1>" + status.substr(0, 3) + "</h1>");
	result.append("<h3>" + status.substr(4, status.size()) + "</h3>");
	result.append("<p>Click <a href=\"index.html\">here</a> to return home.</p>");
	result.append("</body>");
	result.append("</html>");
	return result;
}