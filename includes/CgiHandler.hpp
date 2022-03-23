#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>
#include "Request.hpp"

class CgiHandler {
	private:
	std::map<std::string, std::string> env;
	public:
	CgiHandler(Request &request);
};

#endif