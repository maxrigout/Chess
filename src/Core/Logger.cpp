#include "Logger.h"
#include <iostream>
#include <set>
#include <signal.h>
#include <time.h>

std::set<std::string> s_errorMessages;

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

void Logger::Log(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg)
{
	std::cout << "[" << debugLevel << "][" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Tag(const char* tag, const char* file, const char* function, int lineNumber, const std::string& msg)
{
	std::cout << "[" << tag << "][] - " << msg << std::endl;
}

void Logger::Info(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	// display the level in green
	std::cout << "\x1B[32m" << "[INFO]" << "\x1B[0m" << "[" << GetTime() << "]" << " - " << msg << std::endl;
}

void Logger::Debug(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	// display the level in yellow
	std::cout << "\x1B[33m" << "[DEBUG]" << "\x1B[0m" << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Error(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	// display the level in red
	std::cout << "\x1B[31m" << "[ERROR]" << "\x1B[0m" << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::ErrorOnce(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	if (s_errorMessages.count(msg))
		return;
	s_errorMessages.insert(msg);
	// display the level in red
	std::cout << "\x1B[31m" << "[ERROR ONCE]" << "\x1B[0m" << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
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
