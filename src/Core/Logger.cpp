#include "Logger.h"
#include <iostream>
#include <set>
#include <signal.h>
#include <time.h>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <chrono>

#define COLOR_RESET "\x1B[0m"
#define FG_RED "\x1B[31m"
#define FG_GREEN "\x1B[32m"
#define FG_YELLOW "\x1B[33m"
#define FG_BLUE "\x1B[34m"
#define FG_MAGENTA "\x1B[35m"
#define FG_CYAN "\x1B[36m"
#define FG_WHITE "\x1B[37m"
#define FG_DEFAULT "\x1B[39m"

#define FG_COLORS_SZ 6

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

std::ostream& Logger::s_out = std::cout;
std::mutex Logger::s_mu;
std::set<std::string> Logger::s_errorMessages;

const char* foregroundColors[] = {
	FG_RED,
	FG_BLUE,
	FG_GREEN,
	FG_YELLOW,
	FG_MAGENTA,
	FG_CYAN,
};

size_t get_seconds_since_epoch()
{
    // get the current time
    const auto now     = std::chrono::system_clock::now();

    // transform the time into a duration since the epoch
    const auto epoch   = now.time_since_epoch();

    // cast the duration into seconds
    const auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    
    // return the number of seconds
    return seconds.count();
}

static const char* getThreadColor()
{
	static int fgColorIndex = 0;
	static std::unordered_map<std::thread::id, std::pair<std::string, size_t>> thread_colors;
	auto threadId = std::this_thread::get_id();
	auto ite = thread_colors.find(threadId);
	size_t epoch = get_seconds_since_epoch();
	if (fgColorIndex >= FG_COLORS_SZ)
	{
		// need to replace an existing color
		// find the lowest epoch, and use that color for the thread
		fgColorIndex = 0;
	}
	if (ite == thread_colors.end())
	{
		auto res = thread_colors.emplace(threadId, std::make_pair(foregroundColors[fgColorIndex++], 0));
		ite = res.first;
	}

	ite->second.second = epoch;

	return ite->second.first.c_str();
}

static const char* getThreadColor2()
{
	// TODO: make a version with a vector
	// the index of the vector should be the same as the one in the color table
	// kinda like a registry
	// or order the threads by usage
	return "";
}

std::string Logger::LogThreadId()
{
	std::stringstream ss;
	ss << getThreadColor() << "[thread-" << std::this_thread::get_id() << "]" << COLOR_RESET;
	return ss.str();
}

void Logger::Log_old(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	s_out << "[" << debugLevel << "][" << GetTime() << "]"<< LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Tag_old(const char* tag, const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	s_out << "[" << tag << "][] " << LogThreadId() << "- " << msg << std::endl;
}

void Logger::Trace_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
    Tag_old("TRACE", file, function, lineNumber, msg);
}

void Logger::Verbose_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
    Tag_old("VERBOSE", file, function, lineNumber, msg);
}

void Logger::Debug_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
    // display the level in yellow
    s_out << FG_YELLOW << "[DEBUG]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Info_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	// display the level in green
	s_out << FG_GREEN << "[INFO]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "" << " - " << msg << std::endl;
}

void Logger::Warn_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	s_out << FG_MAGENTA << "[WARN]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "" << " - " << msg << std::endl;
}

void Logger::Error_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	// display the level in red
	s_out << FG_RED << "[ERROR]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::ErrorOnce_old(const char* file, const char* function, int lineNumber, const std::string& msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	if (s_errorMessages.count(msg))
		return;
	s_errorMessages.insert(msg);
	// display the level in red
	s_out << FG_RED << "[ERROR ONCE]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
}

void Logger::Fatal_old(const char* file, const char* function, int lineNumber, std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mu);
	s_out << FG_RED << "[FATAL]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - " << msg << std::endl;
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