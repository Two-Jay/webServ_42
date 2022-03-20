#include "../includes/ConfigParser.hpp"

ConfigParser::ConfigParser(const char* filename)
{
	std::string read;
	std::ifstream fs;

	content.clear();
	fs.open(filename);
	if (fs.is_open())
	{
		while (!fs.eof())
		{
			getline(fs, read);
			content.append(read + '\n');
		}
		fs.close();
	}
}

ConfigParser::~ConfigParser()
{
}

std::vector<Server> ConfigParser::parse()
{
	std::vector<Server> result;
	size_t pre = 0;
	size_t cur = content.find_first_not_of(" \n\t", pre);
	if (cur == std::string::npos)
		exit(print_parse_error());

	while (cur != std::string::npos)
	{
		if (content.substr(cur, 6) != "server")
		{
			exit(print_parse_error());
		}
		cur += 6;
		result.push_back(parse_server(&cur));
	}
	return result;
}

Server ConfigParser::parse_server(size_t *i)
{
	Server result;

	std::cout << "parse_server.. starts with (" << *i << ")" << std::endl;
	size_t pre = content.find_first_not_of(" \t\n", *i);
	if (pre == std::string::npos || content[pre] != '{')
		exit(print_parse_error());
		
	pre++;
	size_t cur = content.find_first_not_of(" \t\n", pre);
	while (cur != std::string::npos)
	{
		pre = content.find_first_not_of(" \t\n", cur);;
		cur = content.find_first_of(" \t\n", pre);
		std::string key = content.substr(pre, cur - pre);
		if (key == "}")
		{
			*i = content.find_first_not_of(" \n\t", cur + 1);
			break;
		}

		if (key == "location")
		{
			parse_location(&cur);
		}
		else
		{
			std::cout << ">> key: " << key << ", ";
			pre = content.find_first_not_of(" \t\n", cur);
			cur = content.find_first_of("\n", pre);
			std::string value = content.substr(pre, cur - pre);
			std::cout << "value: " << value << std::endl;
		}
	}
	std::cout << "parse_server.. finish with (" << *i << ")" << std::endl;
	return result;
}

Location ConfigParser::parse_location(size_t *i)
{
	Location result;

	std::cout << "parse_location.. starts with (" << *i << ")" << std::endl;
	size_t pre = content.find_first_not_of(" \t\n", *i);
	size_t cur = content.find_first_of(" \t\n", pre);
	result.path = content.substr(pre, cur - pre);

	pre = content.find_first_not_of(" \t\n", cur);
	if (pre == std::string::npos || content[pre] != '{')
		exit(print_parse_error());
		
	pre++;
	cur = content.find_first_not_of(" \t\n", pre);
	while (cur != std::string::npos)
	{
		pre = content.find_first_not_of(" \t\n", cur);;
		cur = content.find_first_of(" \t\n", pre);
		std::string key = content.substr(pre, cur - pre);
		if (key == "}")
		{
			*i = cur;
			break;
		}
		else
		{
			std::cout << ">> key: " << key << ", ";
			pre = content.find_first_not_of(" \t\n", cur);
			cur = content.find_first_of("\n", pre);
			std::string value = content.substr(pre, cur - pre);
			std::cout << "value: " << value << std::endl;
			if (set_location_values(&result, key, value) == FAILED)
			{
				exit(print_parse_error());
			}
		}
	}
	std::cout << "parse_location.. finish with (" << *i << ")" << std::endl;
	return result;
}

int ConfigParser::set_server_values(Server *server, const std::string key, const std::string value)
{
	if (key == "server_name")
	{
		server->server_name = value;
	}
	else if (key == "listen")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (int i = 0; i != tmp.size(); i++)
			server->listen_socket.push_back(atoi(tmp[i].c_str()));
	}
	else if (key == "root")
	{
		server->root = value;
	}
	else if (key == "index")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (int i = 0; i != tmp.size(); i++)
			server->index.push_back(tmp[i]);
	}
	else if (key == "allow_methods")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (int i = 0; i != tmp.size(); i++)
			server->allow_methods.push_back(Server::s_to_methodtype(tmp[i]));
	}
	else if (key == "autoindex")
	{
		server->autoindex = value == "on" ? true : false;
	}
	else if (key == "client_body_limit")
	{
		server->client_body_limit = atoi(value.c_str());
	}
	else
	{
		return FAILED;
	}
	return SUCCESS;
}

int ConfigParser::set_location_values(Location *location, const std::string key, const std::string value)
{
	if (key == "root")
	{
		location->root = value;
	}
	else if (key == "index")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (int i = 0; i != tmp.size(); i++)
			location->index.push_back(tmp[i]);
	}
	else if (key == "allow_methods")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (int i = 0; i != tmp.size(); i++)
			location->allow_methods.push_back(Location::s_to_methodtype(tmp[i]));
	}
	else if (key == "cgi_info")
	{
		// location->cgi_info = value;
	}
	else
	{
		return FAILED;
	}
	return SUCCESS;
}

int ConfigParser::print_parse_error()
{
	fprintf(stderr, "[ERROR] config parsing failed.");
	return 1;
}