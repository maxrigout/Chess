#include "Logger.h"
#include <iostream>
#include <set>
#include <signal.h>
#include <time.h>

std::set<std::string> s_errorMessages;

void Logger::Log(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg)
{
	std::cout << "[" << debugLevel << "][" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Info(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	std::cout << "[INFO][" << GetTime() << "]" << " - " << msg << std::endl;
}

void Logger::Debug(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	return Log("DEBUG", file, function, lineNumber, msg.c_str());
}

void Logger::Error(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	if (s_errorMessages.count(msg))
		return;
	s_errorMessages.insert(msg);
	Log("ERROR", file, function, lineNumber, msg.c_str());
}

void Logger::Fatal(const char* file, const char* function, int lineNumber, const std::string& msg)
{

}

std::string Logger::GetTime()
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	// return asctime(timeinfo);
	constexpr size_t maxSize = 100;
	char timeStr[maxSize + 1] = {0};
	strftime(timeStr, maxSize, "%X", timeinfo);
	return timeStr;
}

