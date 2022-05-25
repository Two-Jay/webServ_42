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

int Request::parsing(std::string request)
{
	int i;
	int j;

	std::cout << "> request parsing\n";
	i = request.find_first_of(" ", 0);
	method = request.substr(0, i);
	// nginx에서 curl로 보냈을 때 리퀘스트 메소드가 모두 영문대문자가 아니면 400 에러반환함
	if (is_not_method(method))
		return 400;
	if ((j = request.find_first_of(" ", i + 1)) == std::string::npos)
		return 400;
	path = request.substr(i + 1, j - i - 1);
	headers["HTTP"] = request.substr(j + 1, request.find_first_of("\r", i) - j - 1);
	if (headers["HTTP"] != "HTTP/1.1")
		return 505;
	i = request.find_first_of("\n", j) + 1;
	while (i < request.size())
	{
		if (request[i] == '\r' && request[i + 1] == '\0')
			break;
		if (request[i] == '\r' && request[i + 1] == '\n')
		{
			this->body = request.substr(i + 2, request.size());
			break;
		}
		int deli = request.find_first_of(":", i);
		int end = request.find_first_of("\r\n", deli);
		headers[request.substr(i, deli - i)] = request.substr(deli + 2, end + 2 - deli - 3);
		if (end + 1 == '\0')
			break;
		i = end + 2;
	}
	if (headers["Host"] == "")
		return 400;
	if (headers["Expect"] != "")
		return 417;
	return 0;
}

std::string Request::get_path()
{
	int i = path.find_first_of("?", 0);
	if (i == std::string::npos)
		return path;
	return path.substr(0, i - 1);
}

std::string Request::get_query()
{
	int i = path.find_first_of("?", 0);
	if (i == std::string::npos)
		return "";
	return path.substr(i, path.size() - i);
}

bool Request::is_not_method(const std::string method) {
	if(method.empty())
		return true;
	for(int i = 0; i < method.length(); i++)
	{
		if(!isupper(static_cast<unsigned char>(method[i])))
			return true;
	}
	return false;
}