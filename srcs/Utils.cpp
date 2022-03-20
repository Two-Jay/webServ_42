#include "../includes/Utils.hpp"

int replace(std::string &original, std::string word1, std::string word2)
{
	int pos = original.find(word1);
	int result = 0;
	while (pos != std::string::npos)
	{
		result = 1;
		original = original.replace(pos, word1.length(), word2);
		pos = original.find(word1);
	}
	std::cout << "replace: " << original << "\n";
	return result;
}

std::string dir_listing()
{
	std::string path = "www/html/data/";
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path.c_str());

	std::string result = "<ul>";
	while ((ent = readdir(dir)) != NULL)
	{
		result += "<li><a href=\"" + (std::string)ent->d_name + ">" 
			+ (std::string)ent->d_name + "<\\a><\\li>";
	}
	result += "</ul>";
	std::cout << "dir: " << result << "\n";
	return result;
}

std::vector<std::string> split(std::string input, char delimiter)
{
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;
}