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
		std::string CGI_result;
	public:
		CgiHandler(Request &request);
		char** set_env();
		std::string& get_CGI_result(void);
		void excute_CGI(Request &Request, Location &loc);

		friend std::ostream &operator<<(std::ostream &out, CgiHandler &ch)
		{
			out << "cgi_env\n"; 
			for (std::map<std::string, std::string>::iterator it = ch.env.begin(); it != ch.env.end(); it++) {
				out << "first : " << it->first << " || second : " << it->second << '\n';
			}
			return out;
		}
};

#endif