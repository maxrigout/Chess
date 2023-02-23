#include "TestLogger.h"
#include <algorithm>
#include <iostream>
#include <locale>
#include <codecvt>

std::vector<TestLogger::TestLoggerInstance_T> TestLogger::s_instances;

void TestLogger::BeginExecution(const std::string& testName)
{
	std::cout << "[" << GetTime() << "] executing " << testName << std::endl;
}

void TestLogger::EndExecution(int ellapedTimeMs)
{
	std::cout << "[" << GetTime() << "] finished executing. Took " << "\x1B[33m" << ellapedTimeMs << " ms" << "\x1B[0m\n" << std::endl;
	m_hasEnded = true;
}

void TestLogger::Pass(const std::string& test)
{
	std::cout << "\x1B[42m" << "Ok" << "\x1B[0m - " << test << "\n"; 
}

void TestLogger::Fail(const std::string& test)
{
	std::cout << "\x1B[41m" << "Fail" << "\x1B[0m - " << test << "\n";
}

void TestLogger::Error(const std::string& op, const std::string& actual, const std::string& expected, const std::string& fileName, const std::string& function, int lineNumber) const
{
	// std::cout << "\x1B[31m" << "Test Error at " << file << "::" << lineNumber << " - " << op << " - " << "expected: " << expected << " actual: " << actual << "\x1B[0m\n";
	std::cout << "\t\x1B[31m" << op << " - " << "expected:<" << expected << "> actual:<" << actual << "> at " << fileName << "::" << lineNumber << "\x1B[0m\n";
}

void TestLogger::LogResult(const std::string& op, const std::string& actual, const std::string& expected, const std::string& fileName, int lineNumber)
{
	std::cout << m_id;
}


TestLogger::TestLoggerInstance_T TestLogger::GetLogger()
{
	if (s_instances.size() == 0)
		s_instances.push_back(std::make_shared<TestLogger>(0));
	auto ite = std::find_if(s_instances.begin(), s_instances.end(), [](TestLoggerInstance_T& logger)
	{
		return logger->IsAvailable();
	});
	if (ite != s_instances.end())
		return *ite;
	s_instances.push_back(std::make_shared<TestLogger>(s_instances.size()));
	return s_instances.back();
}

std::string TestLogger::GetTime()
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