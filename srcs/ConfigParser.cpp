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
	{
		exit(print_parse_error());
	}

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
	{
		exit(print_parse_error());
	}
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
	{
		exit(print_parse_error());
	}
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
		}
	}
	std::cout << "parse_location.. finish with (" << *i << ")" << std::endl;
	return result;
}

int ConfigParser::print_parse_error()
{
	fprintf(stderr, "[ERROR] config parsing failed.");
	return(1);
}