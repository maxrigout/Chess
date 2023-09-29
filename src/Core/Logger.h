#pragma once

#include <string>
#include <exception>
// #include <set>

class Logger
{
public:
	// static void Info(const char* file, const char* function, int lineNumber, const char* msg);
	// static void Debug(const char* file, const char* function, int lineNumber, const char* msg);
	// static void Error(const char* file, const char* function, int lineNumber, const char* msg);
	// static void Fatal(const char* file, const char* function, int lineNumber, const char* msg);
	static void Tag(const char* tag, const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Trace(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Verbose(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Warn(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Debug(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Info(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Error(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void ErrorOnce(const char* file, const char* function, int lineNumber, const std::string& msg);
	static void Fatal(const char* file, const char* function, int lineNumber, const std::string& msg);

private:
	static void Log(const char* debugLevel, const char* file, const char* function, int lineNumber, const char* msg);
	static std::string GetTime();
};

#ifndef RELEASE
#define LOG_TAG(tag, msg) Logger::Tag(tag, __FILE__, __func__, __LINE__, msg)
#define LOG_INFO(msg) Logger::Info(__FILE__, __func__, __LINE__, msg)
#define LOG_DEBUG(msg) Logger::Debug(__FILE__, __func__, __LINE__, msg)
#define LOG_ERROR(msg) Logger::Error(__FILE__, __func__, __LINE__, msg)
#define LOG_ERROR_ONCE(msg) Logger::ErrorOnce(__FILE__, __func__, __LINE__, msg)
#define LOG_FATAL(msg) Logger::Fatal(__FILE__, __func__, __LINE__, msg)
#else
#define LOG_TAG(tag, msg)
#define LOG_INFO(tag, msg)
#define LOG_DEBUG(msg)
#define LOG_ERROR(msg)
#define LOG_ERROR_ONCE(msg)
#define LOG_FATAL(msg)
#endif
