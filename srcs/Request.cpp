#include "../includes/Request.hpp"

Request::Request(int client_fd)
{
	client_fd = client_fd;
}

Request::~Request()
{
}

int Request::get_client_fd() {
	return client_fd;
}

std::string Request::get_port() {
	int i = headers["Host"].find_first_of(":", 0);
	return headers["Host"].substr(i + 1, headers["Host"].size() - i - 1);
}

void Request::parsing(std::string request)
{
	std::cout << "parsing: " << request << "\n";
	int i = request.find_first_of(" ", 0);
	method = request.substr(0, i);
	int j = request.find_first_of(" ", i + 1);
	path = request.substr(i + 1, j - i - 1);
	headers["HTTP"] = request.substr(j + 1, request.find_first_of("\r", i) - j - 1);
	i = request.find_first_of("\n", j) + 1;
	while (i < request.size())
	{
		if (request[i] == '\r' && request[i + 1] == '\n')
		{
			this->body = request.substr(4, request.size());
			break;
		}
		int deli = request.find_first_of(":", i);
		int end = request.find_first_of("\r\n", deli);
		headers[request.substr(i, deli - i)] = request.substr(deli + 2, end + 2 - deli - 3);
		i = end + 2;
	}
	std::cout << headers;
}

std::string Request::get_path()
{
	return path;
}

std::string Request::get_query()
{

}