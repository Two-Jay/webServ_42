#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <dirent.h>

int replace(std::string &original, std::string word1, std::string word2);
std::string dir_listing();

#endif