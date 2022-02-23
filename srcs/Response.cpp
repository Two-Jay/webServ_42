#include "../includes/Response.hpp"

Response::Response(/* args */)
{
}

Response::~Response()
{
}

std::string Response::serialize()
{
	std::string result;

	result.append("HTTP/1.1 " + status_code + "\r\n");
	for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
	{
		result.append((*i).first + ":" + (*i).second + "\r\n");	
	}
	result.append("\r\n");
	result.append(body);

	return result;
}