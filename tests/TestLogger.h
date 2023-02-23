#pragma once

#include <string>
#include <vector>
#include <iostream>

class TestLogger
{
public:

	using TestLoggerInstance_T = std::shared_ptr<TestLogger>;

	void BeginExecution(const std::string& msg);
	void EndExecution(int ellapedTimeMs);
	void Pass(const std::string& test);
	void Fail(const std::string& test);
	void Error(const std::string& op, const std::string& actual, const std::string& expected, const std::string& file, const std::string& function, int lineNumber) const;
	void LogResult(const std::string& op, const std::string& actual, const std::string& expected, const std::string& fileName, int lineNumber);

	static std::ostream& Log() { return std::cout; }

	static TestLoggerInstance_T GetLogger();
	TestLogger(int id) : m_id{id}, m_hasEnded{true} {}

private:
	TestLogger(const TestLogger&) = delete;
	int m_id;
	bool m_hasEnded;
	bool IsAvailable() { return m_hasEnded; }
	static std::string GetTime();
	static std::vector<TestLoggerInstance_T> s_instances;
};

#define LOG_INFO(msg) Logger::Info(__FILE__, __func__, __LINE__, msg)
#define LOG_DEBUG(msg) Logger::Debug(__FILE__, __func__, __LINE__, msg)
#define LOG_ERROR(msg) Logger::Error(__FILE__, __func__, __LINE__, msg)
#define LOG_FATAL(msg) Logger::Fatal(__FILE__, __func__, __LINE__, msg)