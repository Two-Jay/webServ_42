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
