#include "../includes/Request.hpp"

Request::Request(int client_fd)
{
	this->client_fd = client_fd;
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

static bool check_protocol(std::map<std::string, std::string>::mapped_type& parsed) {
	if (parsed != "HTTP/1.1") return false;
	return true;
}

static std::string parse_request_body_chunked(std::string& request) {
	std::string ret;
    std::size_t size = 1;
    std::string line = request;
    while (true) {
        line = line.erase(0, 2);
        size = atoi(line.substr(0, line.find("\r\n")).c_str());
		if (size == 0) break ;
        line = line.erase(0, line.find("\r\n") + 2);
        std::string msg = line.substr(0, line.find("\r\n"));
        line.erase(0, msg.size());
		ret += msg;
    }
	return ret;
}

static std::string parse_request_body(std::string& request, int index) {
	return request.substr(index + 2, request.size());
}

static int parse_headers_line(std::map<std::string, std::string>& headers, std::string& request, int index) {
	int deli = request.find_first_of(":", index);
	int end = request.find_first_of("\r\n", deli);
	headers[request.substr(index, deli - index)] = request.substr(deli + 2, end - deli - 2);
	return end;
}

int Request::parsing(std::string request)
{
	unsigned long i;
	int j;

	std::cout << "> request parsing\n";
	std::cout << "request string : [" << request << "]\n";
	i = request.find_first_of(" ", 0);
	method = request.substr(0, i);
	if (method == "PUT")
		return 200;
	if (is_not_method(method))
		return 400;
	if ((unsigned long)(j = request.find_first_of(" ", i + 1)) == std::string::npos)
		return 400;
	path = request.substr(i + 1, j - i - 1);
	headers["HTTP"] = request.substr(j + 1, request.find_first_of("\r", i) - j - 1);
	if (check_protocol(headers["HTTP"]) == false) return 505;
	i = request.find_first_of("\n", j) + 1;
	while (i < request.size())
	{
		if (request[i] == '\r' && request[i + 1] == '\0')
			break;
		if (request[i] == '\r' && request[i + 1] == '\n')
		{
			if (headers["Transfer-Encoding"] == "chunked")
				this->body = parse_request_body_chunked(request);
			else
				this->body = parse_request_body(request, i);
			break;
		}
		int end = parse_headers_line(this->headers, request, i);
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
	unsigned long i = path.find_first_of("?", 0);
	if (i == std::string::npos)
		return path;
	if ((int)i == -1)
		i = path.length();
	return path.substr(0, i);
}

std::string Request::get_query()
{
	unsigned long i = path.find_first_of("?", 0);
	if (i == std::string::npos)
		return "";
	return path.substr(i + 1, path.size() - i);
}

bool Request::is_not_method(const std::string method) {
	if(method.empty())
		return true;
	for(unsigned long i = 0; i < method.length(); i++)
	{
		if(!isupper(static_cast<unsigned char>(method[i])))
			return true;
	}
	return false;
}