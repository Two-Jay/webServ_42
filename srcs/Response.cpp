#include "../includes/Response.hpp"

Response::Response(std::string status)
{
	this->status = status;
}

Response::~Response()
{
}

void Response::append_header(std::string first, std::string second)
{
	headers.insert(std::make_pair(first, second));
}

std::string Response::make_header()
{
	std::string result;

	result.append("HTTP/1.1 " + status + "\r\n");
	for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
	{
		result.append((*i).first + ": " + (*i).second + "\r\n");	
	}
	result.append("\r\n");

	return result;
}

std::string Response::make_error_page()
{
	std::string result;

	result.append("<!DOCTYPE html>");
// 	result.append("<h1>" + status + "<h1>");
// <html>
// 	<head>
// 		<meta charset="UTF-8" />
// 		<title>webserv</title>
// 	</head>
// 	<body>
// 		<h1>42 webserv</h1>
// 		<h3>This is when you finally understand why a URL starts with HTTP</h3>
// 		<hr>
// 		<p>author : yoahn, sunhkim</p>
// 		<p>Click <a href="board.html">here</a> to test html methods</p>
// 		<li>
// 			Subject
// 			<ul>Make nginx-like html server</ul>
// 			<ul>Read Config file to configuration</ul>
// 			<ul>GET, POST, DELETE methods</ul>
// 			<ul>Parse vaild REQUEST and make good RESPONSE with vaild response code</ul>
// 			<ul>Run CGI program(Gateway)</ul>
// 		</li>
// 	</body>
// </html>")
	return "";
}