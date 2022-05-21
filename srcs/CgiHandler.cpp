#include "../includes/CgiHandler.hpp"

#define BUFFER_SIZE 100

CgiHandler::CgiHandler(Request &request)
{
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["SERVER_NAME"] = request.headers["Host"];
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->env["SERVER_PORT"] = request.get_port();
	this->env["REQUEST_METHOD"] = request.method;
	this->env["PATH_INFO"] = request.get_path();
	this->env["PATH_TRANSLATED"] = request.get_path();
	this->env["SCRIPT_NAME"] = request.get_path();
	this->env["QUERY_STRING"] = request.get_query();
	this->env["REMOTE_HOST"] = request.headers["Host"];
	this->env["REMOTE_ADDR"] = get_ip(request.get_client_fd());
	this->env["AUTH_TYPE"] = "";
	this->env["REMOTE_USER"] = "";
	this->env["REMOTE_IDENT"] = "";
	this->env["CONTENT_TYPE"] = request.headers["Content-Type"];
	this->env["CONTENT_LENGTH"] = request.headers["Content-Length"];
}

char** CgiHandler::set_env()
{
	char **envp = new char*[sizeof(char*) * (env.size() + 1)];
	if (!envp)
		return NULL;
	
	int i = 0;
	for(std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
		envp[i] = strdup((it->first + "=" + it->second).c_str());
		++i;
	}
	envp[i] = NULL;
	return envp;
}

std::string& CgiHandler::get_CGI_result(void) {
	return this->CGI_result;
};

void CgiHandler::excute_CGI(Request &Request, Location &loc)
{
	int read_fd[2];
	int write_fd[2];
	int pid;
	int ret1 = pipe(read_fd);
	std::string cgires;

	if (ret1 < 0 || pipe(write_fd) < 0) return ;
	pid = fork();
	if (pid < 0) return ;
	else if (pid == 0)
	{
		dup2(write_fd[0], STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		close(write_fd[1]);
		close(read_fd[0]);
		char **env = set_env();
		std::string extension = Request.get_path().substr(Request.get_path().find(".") + 1);
		char *av[3] = { const_cast<char*>(loc.getCgiBinary(extension).c_str()), 
		const_cast<char*>(loc.root.c_str()), NULL};
		if (env)
			ret1 = execve(av[0], av, env);
		exit(1);
	}
	else
	{
		close(write_fd[0]);
		close(read_fd[1]);
		char read_buf[BUFFER_SIZE];
		memset(read_buf, 0, BUFFER_SIZE);
		int nbytes = 1;
		while (nbytes > 0)
		{
			nbytes = read(read_fd[0], read_buf, BUFFER_SIZE);
			if (nbytes <= 0) break ;
			cgires.append(read_buf);
			memset(read_buf, 0, BUFFER_SIZE);
		}
		std::cout << cgires << '\n';
		this->CGI_result = cgires;
	}
}