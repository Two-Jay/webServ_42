#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <signal.h>
#include "Request.hpp"
#include "Location.hpp"

#define BUFFER_SIZE 100

class CgiHandler {
	private:
		std::map<std::string, std::string> env;
		FILE *resource_p;
		std::string file_resource;
		size_t file_size;

	public:
		CgiHandler(Request &request, Location& loc);
		char** set_env();
		int excute_CGI(Request &Request, Location &loc);

		friend std::ostream &operator<<(std::ostream &out, CgiHandler &ch)
		{
			out << "cgi_env\n"; 
			for (std::map<std::string, std::string>::iterator it = ch.env.begin(); it != ch.env.end(); it++) {
				out << "first : " << it->first << " || second : " << it->second << '\n';
			}
			return out;
		}
		std::string get_target_file_fullpath(Request& req, Location& loc);
};

#endif