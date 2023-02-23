#pragma once

#include "TestLogger.h"
#include "TestTimer.h"

#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <string>

struct TestingContext
{

};

struct ExecutionResult
{
	std::string compOperator;
	std::string expected;
	std::string actual;
	bool passed;
	std::string fileName;
	int lineNumber;
};

class TestRunner;

class TestClass
{
public:
	virtual ~TestClass() = default;
	virtual const char* GetName() const = 0;
	virtual void ExecutionDetails() = 0;

	bool DidPass() const { return m_didPass; }

	void IsTrue(bool actual, const char* fileName, int lineNumber)
	{
		ExecutionResult result;
		result.compOperator = "IsTrue";
		result.expected = "true";
		result.actual = actual ? "true" : "false";
		result.passed = actual;
		result.fileName = fileName;
		result.lineNumber = lineNumber;
		m_results.push_back(result);
		m_didPass = m_didPass && result.passed;
	}

	void IsFalse(bool actual, const char* fileName, int lineNumber)
	{
		ExecutionResult result;
		result.compOperator = "IsFalse";
		result.expected = "false";
		result.actual = actual ? "true" : "false";
		result.passed = !actual;
		result.fileName = fileName;
		result.lineNumber = lineNumber;
		m_results.push_back(result);
		m_didPass = m_didPass && result.passed;
	}

	template <typename T>
	void IsEqual(T expected, T actual, const char* fileName, int lineNumber)
	{
		ExecutionResult result;
		result.compOperator = "IsEqual";
		result.expected = std::to_string(expected);
		result.actual = std::to_string(actual);
		result.passed = (expected == actual);
		result.fileName = fileName;
		result.lineNumber = lineNumber;
		m_results.push_back(result);
		m_didPass = m_didPass && result.passed;
	}

	template <typename T>
	void IsNotEqual(T expected, T actual, const char* fileName, int lineNumber)
	{
		ExecutionResult result;
		result.compOperator = "IsNotEqual";
		result.expected = std::to_string(expected);
		result.actual = std::to_string(actual);
		result.passed = (expected != actual);
		result.fileName = fileName;
		result.lineNumber = lineNumber;
		m_results.push_back(result);
		m_didPass = m_didPass && result.passed;
	}

private:
	int GetExecutionTimeMs() const { return executionTimeUs * 1000; }
	void Execute(std::shared_ptr<TestLogger> logger)
	{
		TestTimer timer;
		timer.Start();
		ExecutionDetails();
		timer.Stop();
		executionTimeUs = timer.GetEllapsedTime();
	}

	const std::vector<ExecutionResult>& GetTestResults() const
	{
		return m_results;
	}

	int executionTimeUs;
	std::vector<ExecutionResult> m_results;
	bool m_didPass = true;;
	friend class TestRunner;

};

// TODO configuration (such as multi-threaded execution)

class TestRunner
{
public:
	using TestClass_T = std::shared_ptr<TestClass>;
	using TestFunction_T = std::function<void(void)>;

	template <typename T> static bool RegisterType() { return Get().RegisterType_Impl<T>(); }
	static bool RegisterTestFunction(TestFunction_T fn) { return Get().RegisterTestFunction_Impl(fn); }
	static bool RegisterTestClass(TestClass_T c) { return Get().RegisterTestClass_Impl(c); }
	static void Run(const TestingContext& ctx) { return Get().Run_Impl(ctx); }
	static void PrintReport() { return Get().PrintReport_Impl(); }

private:
	static TestRunner& Get() { static TestRunner s_instance; return s_instance; }

	template<typename T> bool RegisterType_Impl()
	{
		return RegisterTestClass_Impl(new T());
	}

	bool RegisterTestFunction_Impl(TestFunction_T fn)
	{
		m_testFunctions.push_back(fn);
		return true;
	}

	bool RegisterTestClass_Impl(TestClass_T c)
	{
		m_testClasses.push_back(c);
		return true;
	}

	void Run_Impl(const TestingContext& ctx)
	{
		// log: running m_testFunctions.size() tests
		TestLogger::Log() << "running " << m_testClasses.size() << " tests\n";
		for (auto& test : m_testClasses)
		{
			// acquire new log
			std::shared_ptr<TestLogger> logger = TestLogger::GetLogger();
			logger->BeginExecution(test->GetName());
			test->Execute(logger);
			logger->EndExecution(test->GetExecutionTimeMs());
			m_testResults.emplace(test->GetName(), test->GetTestResults());
		}
	}

	void Run2_Impl(const TestingContext& ctx)
	{
		// log: running m_testFunctions.size() tests
		for (const auto& test : m_testFunctions)
		{
			// acquire new log
			// log: execution of "testName" stated at [time]
			test();
			// log: execution finished at [time] (took [elapsed time seconds])
		}
	}

	void PrintReport_Impl() const
	{
		std::shared_ptr<TestLogger> logger = TestLogger::GetLogger();
		for (auto& test : m_testClasses)
		{
			auto results = m_testResults.at(test->GetName());
			if (test->DidPass())
				logger->Pass(test->GetName());
			else
				logger->Fail(test->GetName());
			for (const auto& result : results)
			{
				if (!result.passed)
					logger->Error(result.compOperator, result.actual, result.expected, result.fileName, "", result.lineNumber);
			}
		}
	}

	std::vector<TestFunction_T> m_testFunctions;
	std::vector<TestClass_T> m_testClasses;
	std::map<std::string, std::vector<ExecutionResult>> m_testResults;
};

#define IS_TRUE(value) this->IsTrue(value, __FILE__, __LINE__)
#define IS_FALSE(value) this->IsFalse(value, __FILE__, __LINE__)
#define IS_EQUAL(expected, actual) this->IsEqual(expected, actual, __FILE__, __LINE__)
#define IS_NOT_EQUAL(expected, actual) this->IsNotEqual(expected, actual, __FILE__, __LINE__)
#define IS_STRICTLY_GREATER_THAN(value1, value2)
#define IS_STRICTLY_LESS_THAN(value1, value2)
#define IS_GREATER_THAN(value1, value2)
#define IS_LESS_THAN(value1, value2)
#define EXCPETION_IS_THROWN(msg)

#define TEST_FUNCTION(name) class name : public TestClass {\
public:\
	virtual ~name() = default;\
	virtual void ExecutionDetails() override;\
	virtual const char* GetName() const override;\
private:\
	static bool s_isRegistered;\
};\
const char* name::GetName() const { return #name; }\
bool name::s_isRegistered = TestRunner::RegisterTestClass(std::make_shared<name>());\
void name::ExecutionDetails() 