#pragma once
//#include <string>

class Logger
{
public:
	static void info(const char* msg);
	static void error(const char* msg);
	static void debug(const char* msg);

private:
	static bool timestamped;
};