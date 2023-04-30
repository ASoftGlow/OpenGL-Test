#pragma once

//#include <string>

class Logger
{
public:
	Logger();
	Logger(bool timestamped);

	void info(const char* msg);
	void error(const char* msg);
	void debug(const char* msg);

private:
	bool timestamped;
};