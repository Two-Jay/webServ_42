#include "../includes/Location.hpp"

Location::Location(/* args */)
{
	path = "";
	root = "";
}

Location::~Location()
{
}

void Location::print_location_info()
{
	std::cout << "----------------- Location Info -----------------" << std::endl;
	std::cout << "> path: " << path << std::endl;
	std::cout << "> root: " << root << std::endl;
	std::cout << "> index: " << index << std::endl;
	std::cout << "> allow_methods: " << allow_methods << std::endl;
	for (std::map<std::string, std::string>::iterator i = cgi_info.begin(); i != cgi_info.end(); i++)
	{
		std::cout << "> cgi_info: " << (*i).first << ", " << (*i).second << std::endl;
	}
}

MethodType Location::s_to_methodtype(std::string str)
{
	if (str == "GET")
	{
		return GET;
	}
	else if (str == "POST")
	{
		return POST;
	}
	else if (str == "DELETE")
	{
		return DELETE;
	}
	return INVALID;
}