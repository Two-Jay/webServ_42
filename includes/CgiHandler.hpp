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

	friend std::ostream &operator<<(std::ostream &out, const CgiHandler &ch)
	{
		out << "cgi_env\n"; 
		for (auto it = ch.env.begin(); it != ch.env.end(); it++) {
			out << "first : " << it->first << " || second : " << it->second << '\n';
		}
		return out;
	}
};

#endif