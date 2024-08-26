#pragma once

#include <string>
#include <string_view>
#include <exception>
#include <iostream>
#include <set>
#include <mutex>
// TODO: the logging functions need to support varargs

#define COLOR_RESET "\x1B[0m"
#define FG_RED "\x1B[31m"
#define FG_GREEN "\x1B[32m"
#define FG_YELLOW "\x1B[33m"
#define FG_BLUE "\x1B[34m"
#define FG_MAGENTA "\x1B[35m"
#define FG_CYAN "\x1B[36m"
#define FG_WHITE "\x1B[37m"
#define FG_DEFAULT "\x1B[39m"

class Logger
{
public:
	static void Tag_old(const char* tag, const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Trace_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Verbose_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Debug_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Info_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Warn_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void Error_old(const char* file, const char* function, int lineNumber, std::string_view msg);
	static void ErrorOnce_old(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Fatal_old(const char* file, const char* function, int lineNumber, std::string_view msg);

	template <typename... TArgs>
	static void Tag(const char* tag, const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		s_out << "[" << tag << "][] " << LogThreadId() << "[" << file << "::" << lineNumber << "]" << "- ";
		PrintArgs(args...);
	}

	template <typename... TArgs>
	static void Trace(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		Tag("TRACE", file, function, lineNumber, args...);
	}

	template <typename... TArgs>
	static void Verbose(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		Tag("VERBOSE", file, function, lineNumber, args...);
	}

	template <typename... TArgs>
	static void Debug(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		// display the level in yellow
		s_out << FG_YELLOW << "[DEBUG]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - ";
		PrintArgs(args...);
	}

	template <typename... TArgs>
	static void Info(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		// display the level in green
		s_out << FG_GREEN << "[INFO]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << " - ";
		PrintArgs(args...);
	}

	template <typename... TArgs>
	static void Warn(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		s_out << FG_MAGENTA << "[WARN]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << " - ";
		PrintArgs(args...);
	}

	template <typename... TArgs>
	static void Error(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		// display the level in red
		s_out << FG_RED << "[ERROR]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - ";
		PrintArgs(args...);
	}

	template <typename... TArgs>
	static void Fatal(const char* file, const char* function, int lineNumber, TArgs... args)
	{
		std::lock_guard<std::mutex> lock(s_mu);
		s_out << FG_RED << "[FATAL]" << COLOR_RESET << "[" << GetTime() << "]" << LogThreadId() << "[" << file << "::" << function << "::" << lineNumber << "] - ";
		PrintArgs(args...);
	}

private:
	static std::ostream& s_out;
	static std::mutex s_mu;
	static std::set<std::string> s_errorMessages;

	static void Log_old(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg);
	static std::string GetTime();
	static std::string LogThreadId();

	template <typename T>
	static void PrintArg(const T& arg)
	{
		s_out << arg;
	}

	static void PrintArgs()
	{
		s_out << std::endl;
	}

	template <typename T, typename... TArgs>
	static void PrintArgs(const T& arg, TArgs... rest)
	{
		PrintArg(arg);
		return PrintArgs(rest...);
	}
};

#ifndef RELEASE
#define LOG_TAG_old(tag, msg) Logger::Tag_old(tag, __FILE__, __func__, __LINE__, msg)
#define LOG_TRACE_old(msg) Logger::Trace_old(__FILE__, __func__, __LINE__, msg)
#define LOG_VERBOSE_old(msg) Logger::Verbose_old(__FILE__, __func__, __LINE__, msg)
#define LOG_DEBUG_old(msg) Logger::Debug_old(__FILE__, __func__, __LINE__, msg)
#define LOG_INFO_old(msg) Logger::Info_old(__FILE__, __func__, __LINE__, msg)
#define LOG_WARN_old(msg) Logger::Warn_old(__FILE__, __func__, __LINE__, msg)
#define LOG_ERROR_old(msg) Logger::Error_old(__FILE__, __func__, __LINE__, msg)
#define LOG_ERROR_ONCE_old(msg) Logger::ErrorOnce_old(__FILE__, __func__, __LINE__, msg)
#define LOG_FATAL_old(msg) Logger::Fatal_old(__FILE__, __func__, __LINE__, msg)

#define LOG_TAG(tag, ...) Logger::Tag(tag, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_TRACE(...) Logger::Trace(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_VERBOSE(...) Logger::Verbose(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::Debug(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) Logger::Info(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) Logger::Warn(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Error(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_ERROR_ONCE(msg) Logger::ErrorOnce_old(__FILE__, __func__, __LINE__, msg)
#define LOG_FATAL(...) Logger::Fatal(__FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define LOG_TAG_old(tag, msg)
#define LOG_TRACE_old(msg)
#define LOG_VERBOSE_old(msg)
#define LOG_DEBUG_old(msg)
#define LOG_INFO_old(msg)
#define LOG_WARN_old(msg)
#define LOG_ERROR_old(msg)
#define LOG_ERROR_ONCE_old(msg)
#define LOG_FATAL_old(msg)

#define LOG_TAG(tag, ...)
#define LOG_TRACE(...)
#define LOG_VERBOSE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_ERROR_ONCE(...)
#define LOG_FATAL(...)
#endif


#undef COLOR_RESET
#undef FG_RED
#undef FG_GREEN
#undef FG_YELLOW
#undef FG_BLUE
#undef FG_MAGENTA
#undef FG_CYAN
#undef FG_WHITE
#undef FG_DEFAULT