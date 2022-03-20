#include "../includes/Location.hpp"

Location::Location(/* args */)
{
}

Location::~Location()
{
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