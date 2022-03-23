#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <dirent.h>

int replace(std::string &original, std::string word1, std::string word2);
std::string dir_listing();
std::vector<std::string> split(std::string input, char delimiter);

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &op)
{
	for (int i = 0; i < op.size(); i++)
	{
		out << op[i] << "(" << i << ") ";
	}
	return out;
}

#endif