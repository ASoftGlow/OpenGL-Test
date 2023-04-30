#include "Logger.h"

#include <iostream>

Logger::Logger() : timestamped(false)
{

}
Logger::Logger(bool timestamped)
{
	this->timestamped = timestamped;
}

void Logger::info(const char* msg)
{
	std::cout << "[INFO] " << msg << std::endl;
}

void Logger::debug(const char* msg)
{
	std::cout << "[DEBUG] " << msg << std::endl;
}

void Logger::error(const char* msg)
{
	std::cout << "[ERROR] " << msg << std::endl;
}