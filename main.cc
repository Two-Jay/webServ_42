#include <iostream>
#include <string>

static std::string parse_request_body_chunked(std::string& request) {
	std::string ret;
    std::size_t size = 1, whole_body_size = 0;
    std::string line = request;
    while (true) {
        line = line.erase(0, 2);
        size = atoi(line.substr(0, line.find("\r\n")).c_str());
		if (size == 0) break ;
        line = line.erase(0, line.find("\r\n") + 2);
        std::string msg = line.substr(0, line.find("\r\n"));
        line.erase(0, msg.size());
		whole_body_size += size;
		ret += msg;
    }
	return ret;
}

int main(void) {
    std::string request = "\r\n12\r\nhello world!\r\n5\r\nhello\r\n13\r\ncheck sum yor\r\n0\r\n\r\n";
    int i = 0;

    std::size_t size = 1;
    std::string line = parse_request_body_chunked(request);
    std::cout << line << std::endl;
    std::cout << line.size() << std::endl;
    return 0;
}