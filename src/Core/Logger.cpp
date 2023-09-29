#include "Logger.h"
#include <iostream>
#include <set>
#include <signal.h>
#include <time.h>

std::set<std::string> s_errorMessages;

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

#define COLOR_RESET "\x1B[0m"
#define FG_RED "\x1B[31m"
#define FG_GREEN "\x1B[32m"
#define FG_YELLOW "\x1B[33m"
#define FG_BLUE "\x1B[34m"
#define FG_MAGENTA "\x1B[35m"
#define FG_CYAN "\x1B[36m"
#define FG_WHITE "\x1B[37m"
#define FG_DEFAULT "\x1B[39m"

void Logger::Log(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg)
{
	std::cout << "[" << debugLevel << "][" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Tag(const char* tag, const char* file, const char* function, int lineNumber, const std::string& msg)
{
	std::cout << "[" << tag << "][] - " << msg << std::endl;
}

void Logger::Trace(const char* file, const char* function, int lineNumber, const std::string& msg)
{
    Tag("TRACE", file, function, lineNumber, msg);
}

void Logger::Verbose(const char* file, const char* function, int lineNumber, const std::string& msg)
{
    Tag("VERBOSE", file, function, lineNumber, msg);
}

void Logger::Debug(const char* file, const char* function, int lineNumber, const std::string& msg)
{
    // display the level in yellow
    std::cout << FG_YELLOW << "[DEBUG]" << COLOR_RESET << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Info(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	// display the level in green
	std::cout << FG_GREEN << "[INFO]" << COLOR_RESET << "[" << GetTime() << "]" << " - " << msg << std::endl;
}

void Logger::Warn(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	std::cout << FG_MAGENTA << "[WARN]" << COLOR_RESET << "[" << GetTime() << "]" << " - " << msg << std::endl;
}

void Logger::Error(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	// display the level in red
	std::cout << FG_RED << "[ERROR]" << COLOR_RESET << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::ErrorOnce(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	if (s_errorMessages.count(msg))
		return;
	s_errorMessages.insert(msg);
	// display the level in red
	std::cout << FG_RED << "[ERROR ONCE]" << COLOR_RESET << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Fatal(const char* file, const char* function, int lineNumber, const std::string& msg)
{
		std::cout << FG_RED << "[FATAL]" << COLOR_RESET << "[" << GetTime() << "][" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
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
