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
	for (std::pair<std::string, std::string> header : headers)
	{
		result.append(header.first + ":" + header.second + "\r\n");	
	}
	result.append("\r\n");
	result.append(body);

	return result;
}