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
	// bool progressing = true;

	// while (progressing)
	// {
			
	// }

	std::cout << content << std::endl;

	return result;
}