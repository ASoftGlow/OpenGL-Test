#include "logger.h"

#include <iostream>

void Logger::info(const char* msg)
{
	std::cout << "[INFO] " << msg << std::endl;
}

void Logger::debug(const char* msg)
{
#if _DEBUG
	std::cout << "[DEBUG] " << msg << std::endl;
#endif
}

void Logger::error(const char* msg)
{
	std::cout << "[ERROR] " << msg << std::endl;
}