#include "logger/logger.h"

void Logger::log(const std::string &message, bool isError)
{
	if (isError)
	{
		std::cerr << "ERROR:" << message << '\n';
	}
	else
	{
		std::cout << "INFO: " << message << '\n';
	}
}
