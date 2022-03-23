#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request &request) {
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["SERVER_NAME"] = request.headers["Host"];
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->env["SERVER_PORT"] = request.headers[""];
	this->env["REQUEST_METHOD"] = request.headers["Request Method"];
	this->env["PATH_INFO"] = request.getPath();
	this->env["PATH_TRANSLATED"] = request.getPath();
	this->env["SCRIPT_NAME"] = request.getPath();
	this->env["QUERY_STRING"] = request.getQuery();
	this->env["REMOTE_HOST"] = request.headers["Host"];
	this->env["REMOTE_ADDR"] = ;
	this->env["AUTH_TYPE"] = ;
	this->env["REMOTE_USER"];
	this->env["REMOTE_IDENT"];
	this->env["CONTENT_TYPE"];
	this->env["CONTENT_LENGTH"];
}