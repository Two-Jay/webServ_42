#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <map>
#include <sstream>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int replace(std::string &original, std::string word1, std::string word2);
std::string dir_listing();
std::vector<std::string> split(std::string input, char delimiter);
std::string get_ip(int client_fd);
std::string get_current_date_GMT(void);
size_t StringToHexNumber(std::string input);

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &op)
{
	if (op.size() == 0)
	{
		out << "(empty)";
		return out;
	}

	for (unsigned long i = 0; i < op.size(); i++)
	{
		out << op[i] << "(" << i << ") ";
	}
	return out;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &out, const std::map<T1, T2> &op)
{
	if (op.size() == 0)
	{
		out << "(empty)";
		return out;
	}

	for (typename std::map<T1, T2>::const_iterator i = op.begin(); i != op.end(); i++)
	{
		out << "[ " << (*i).first << " ] = " << (*i).second << "\n";
	}
	return out;
}

template <typename T>
std::string NumberToString ( T Number )
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

#endif