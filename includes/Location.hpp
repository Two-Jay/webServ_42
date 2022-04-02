#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "DataStructs.hpp"
#include "Utils.hpp"

class Location
{
public:
	std::string path;
	std::string root;
	std::vector<std::string> index;
	std::vector<MethodType> allow_methods;
	std::map<std::string, std::string> cgi_info;

public:
	Location();
	~Location();

	void print_location_info();
	static MethodType s_to_methodtype(std::string str);
};

#endif