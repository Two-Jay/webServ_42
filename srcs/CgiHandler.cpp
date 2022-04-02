#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request &request) {
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["SERVER_NAME"] = request.headers["Host"];
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->env["SERVER_PORT"] = request.get_port();
	this->env["REQUEST_METHOD"] = request.headers["Method"];
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