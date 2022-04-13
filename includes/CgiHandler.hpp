#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>
#include <unistd.h>
#include "Request.hpp"
#include "Location.hpp"

class CgiHandler {
	private:
	std::map<std::string, std::string> env;
	public:
	CgiHandler(Request &request);
	char** set_env();
	int cgi_exec(Request &Request, Location &loc);
};

#endif