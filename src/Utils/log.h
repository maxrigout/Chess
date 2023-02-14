#pragma once
#pragma warning(disable:4996)
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <typeindex>


// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html

class Logger
{
public:

	// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
	struct LoggerConfig
	{
		// ostream
		bool time;
		bool function_name; // macro: __func__ on unix, __FUNCTION__ on windows
		bool line_number; // macro: __LINE__
		bool file; // macro: __FILE__

	};

	struct Format
	{
		std::string patter;
		std::vector<std::string> values;
	};

	// add conditions to the logger methods to easily disable them

	static void Info();
	static void Debug();
	static void Warn();
	static void Error();
	static void Fatal();
	static void Custom(const std::string& name);

	static void ConfigureInfo(const LoggerConfig& conf);
	static void ConfigureWarn(const LoggerConfig& conf);
	static void ConfigureError(const LoggerConfig& conf);
	static void ConfigureFatal(const LoggerConfig& conf);

	static void ConfigureCustom(const std::string& logger, const LoggerConfig& conf);

	// Logger::Info("msg");
	// Logger::Custom("name", "msg");


private:
	Logger& GetInstance();

	void Info_Impl() const;
	void Warn_Impl() const;
	void Error_Impl() const;
	void Fatal_Impl() const;
	void Custom_Impl(const std::string& name) const;

	std::string GetTime() const
	{
		time_t rawtime;
		tm timeinfo;
		time(&rawtime);
		timeinfo = *localtime(&rawtime);
		std::string time = asctime(&timeinfo);
		return time.substr(0, time.length() - 1);
	}

	void ConfigureInfo_Impl(const LoggerConfig& conf);
	void ConfigureWarn_Impl(const LoggerConfig& conf);
	void ConfigureError_Impl(const LoggerConfig& conf);
	void ConfigureFatal_Impl(const LoggerConfig& conf);

	void ConfigureCustom_Impl(const std::string& logger, const LoggerConfig& conf);
};

struct PrintableValue
{
	using bool_t = bool;
	using char_t = char;
	using int_t = int64_t;
	using uint_t = uint64_t;
	using dec_t = double;
	using text_t = char*;

#define PRINTABLE_BOOL_TYPE bool
#define PRINTABLE_CHAR_TYPE char
#define PRINTABLE_INTEGER_TYPE int
#define PRINTABLE_UNSIGNED_INTEGER_TYPE unsigned int
#define PRINTABLE_DECIMAL_TYPE double
#define PRINTABLE_TEXT_TYPE char*

	PrintableValue(bool value)
	{
		type = Type::BOOL;
		sz = sizeof(bool);
		pData = malloc(sz);
		(*(bool*)pData) = value;
	}

	PrintableValue(char value)
	{
		type = Type::CHAR;
		sz = sizeof(char);
		pData = malloc(sz);
		(*(char*)pData) = value;
	}

	PrintableValue(int value)
	{
		type = Type::INTEGER;
		sz = sizeof(int_t);
		pData = malloc(sz);
		(*(int_t*)pData) = (int_t)value;
	}

	PrintableValue(unsigned int value)
	{
		type = Type::UNSIGNED_INTEGER;
		sz = sizeof(uint_t);
		pData = malloc(sz);
		(*(uint_t*)pData) = (uint_t)value;
	}
	PrintableValue(size_t value)
	{
		type = Type::SIZE_T;
		sz = sizeof(size_t);
		pData = malloc(sz);
		(*(size_t*)pData) = (size_t)value;
	}

	PrintableValue(float value)
	{
		type = Type::DECIMAL;
		sz = sizeof(dec_t);
		pData = malloc(sz);
		(*(dec_t*)pData) = (dec_t)value;
	}

	PrintableValue(double value)
	{
		type = Type::DECIMAL;
		sz = sizeof(dec_t);
		pData = malloc(sz);
		(*(dec_t*)pData) = (dec_t)value;
	}

	PrintableValue(const char* value)
	{
		type = Type::TEXT;
		sz = strlen(value) + 1;
		pData = malloc(sz);
		std::memcpy(pData, value, sz);
	}
	PrintableValue(char* value)
	{
		type = Type::TEXT;
		sz = strlen(value) + 1;
		pData = malloc(sz);
		std::memcpy(pData, value, sz);
	}

	PrintableValue(const std::string& value)
	{
		type = Type::TEXT;
		sz = value.length() + 1;
		pData = malloc(sz);
		std::memcpy(pData, value.c_str(), sz);
	}

	template <typename T>
	PrintableValue(T* pointer)
	{
		type = Type::POINTER;
		sz = sizeof(T*);
		pData = malloc(sz);
		(*(size_t*)pData) = (size_t)pointer;
	}

	PrintableValue(const PrintableValue& other)
	{
		type = other.type;
		sz = other.sz;
		pData = malloc(sz);
		std::memcpy(pData, other.pData, sz);
	}

	~PrintableValue()
	{
		free(pData);
	}

	void* pData = nullptr;
	size_t sz = 0;
	enum class Type : uint8_t
	{
		UNKNOWN,
		BOOL,
		CHAR,
		TEXT,
		INTEGER,
		UNSIGNED_INTEGER,
		DECIMAL,
		SIZE_T,
		POINTER,
	} type = Type::UNKNOWN;
};

std::string operator+(const std::string& lhs, const PrintableValue& printable);
std::string& operator+=(std::string& lhs, const PrintableValue& printable);

void PrintAdvancedInt(const char* msg, ...);
void PrintPrintables(const char* msg);
void PrintPrintables(const char* msg, const std::vector<PrintableValue>& values);
void PrintPrintablesToFile(const char* fileName, const char* msg, const std::vector<PrintableValue>& values);
void UnpackParams(std::vector<PrintableValue>& values);

template <typename T, typename... TArgs>
void UnpackParams(std::vector<PrintableValue>& values, T value, TArgs... rest)
{
	values.push_back(PrintableValue(value));
	return UnpackParams(values, rest...);
}

template <typename... TArgs>
void PrintAdvanced(const char* msg, TArgs... args)
{
	std::vector<PrintableValue> values;
	UnpackParams(values, args...);
	PrintPrintables(msg, values);
}
template <typename... TArgs>
void PrintAdvanced(const std::string& msg, TArgs... args)
{
	std::vector<PrintableValue> values;
	UnpackParams(values, args...);
	PrintPrintables(msg.c_str(), values);
}

template <typename... TArgs>
void PrintAdvancedToFile(const char* fileName, const char* msg, TArgs... args)
{
	std::vector<PrintableValue> values;
	UnpackParams(values, args...);
	PrintPrintablesToFile(fileName, msg, values);
}
template <typename... TArgs>
void PrintAdvancedToFile(const char* fileName, const std::string& msg, TArgs... args)
{
	std::vector<PrintableValue> values;
	UnpackParams(values, args...);
	PrintPrintablesToFile(fileName, msg.c_str(), values);
}



// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#define LOG(msg, ...) PrintAdvanced(msg, ##__VA_ARGS__)
#define LOG_FN(msg, ...) PrintAdvanced('[' + std::string(__func__) + ']' + msg, ##__VA_ARGS__)
#define LOG_FILE(fileName, msg, ...) PrintAdvancedToFile(fileName, msg, ##__VA_ARGS__)

#ifdef LOG_IMPL
#undef LOG_IMPL

#include <cstdarg>
#include <iostream>
#include <mutex>
#include <string_view>
#include <set>
#include <fstream>

static std::mutex mu_console;
static std::set<std::string> logInProgress;

void PrintAdvancedInt(const char* msg, ...)
{
	va_list values;
	va_start(values, msg);
	int token_index = 0;
	std::string token = "{0}";
	std::string message = msg;

	size_t index = message.find(token);

	while (index != std::string::npos)
	{
		std::string before = message.substr(0, index);
		std::string after = message.substr(index + token.length());
		message = before + std::to_string(va_arg(values, int)) + after;
		token = "{" + std::to_string(++token_index) + "}";
		index = message.find(token);
	}

	va_end(values);

	std::lock_guard<std::mutex> lock(mu_console);
	std::cout << message << std::endl;
}

void UnpackParams(std::vector<PrintableValue>& values)
{
	return;
}

std::string GetMessage(const char* msg, const std::vector<PrintableValue>& values)
{
	// TODO: handle binay, hexadecimal etc...
	// {0b} for binary {0x} for hex (std::hex) {0o} of oct
	std::string message(msg);

	for (size_t i = 0; i < values.size(); ++i)
	{
		std::string token('{' + std::to_string(i) + '}');
		size_t index = message.find(token);
		if (index == std::string::npos)
			continue;
		std::string before(message.substr(0, index));
		std::string after(message.substr(index + token.length()));
		message = before + values[i] + after;
	}
	return message;
}

void PrintPrintables(const char* msg)
{
	std::lock_guard<std::mutex> lock(mu_console);
	std::cout << msg << std::endl;
}
void PrintPrintables(const char* msg, const std::vector<PrintableValue>& values)
{
	std::string message = GetMessage(msg, values);

	std::lock_guard<std::mutex> lock(mu_console);
	std::cout << message << std::endl;
}

void PrintPrintablesToFile(const char* fileName, const char* msg, const std::vector<PrintableValue>& values)
{
	std::string message = GetMessage(msg, values);
	int flags = std::fstream::out | std::fstream::ate | std::fstream::app;
	if (logInProgress.count(fileName) == 0)
	{
		flags = std::fstream::out | std::fstream::trunc;
		logInProgress.emplace(fileName);
	}
	std::fstream f(fileName, flags);
	if (!f.is_open())
	{
		PrintPrintables("unable to save to file");
		return;
	}
	f << message << std::endl;
	f.close();
}

std::string operator+(const std::string& lhs, const PrintableValue& printable)
{
	switch (printable.type)
	{
	case PrintableValue::Type::BOOL:
		return lhs + (*(PrintableValue::bool_t*)(printable.pData) ? "true" : "false");

	case PrintableValue::Type::CHAR:
		return lhs + (*(PrintableValue::char_t*)(printable.pData));

	case PrintableValue::Type::INTEGER:
		return lhs + std::to_string((*(PrintableValue::int_t*)(printable.pData)));

	case PrintableValue::Type::SIZE_T:
		return lhs + std::to_string((*(size_t*)(printable.pData)));

	case PrintableValue::Type::POINTER:
	{
		std::stringstream ss;
		ss << "0x" << std::hex << (*(size_t*)(printable.pData));
		return lhs + ss.str();
	}

	case PrintableValue::Type::UNSIGNED_INTEGER:
		return lhs + std::to_string((*(PrintableValue::uint_t*)(printable.pData)));

	case PrintableValue::Type::DECIMAL:
		return lhs + std::to_string((*(PrintableValue::dec_t*)(printable.pData)));

	case PrintableValue::Type::TEXT:
		return lhs + (char*)(printable.pData);

	case PrintableValue::Type::UNKNOWN:
		break;
	}
	return lhs;
}
std::string& operator+=(std::string& lhs, const PrintableValue& printable)
{
	switch (printable.type)
	{
	case PrintableValue::Type::BOOL:
		return lhs += (*(PrintableValue::bool_t*)(printable.pData) ? "true" : "false");

	case PrintableValue::Type::CHAR:
		return lhs += (*(PrintableValue::char_t*)(printable.pData));

	case PrintableValue::Type::INTEGER:
		return lhs += std::to_string((*(PrintableValue::int_t*)(printable.pData)));

	case PrintableValue::Type::SIZE_T:
		return lhs += std::to_string((*(size_t*)(printable.pData)));

	case PrintableValue::Type::POINTER:
	{
		std::stringstream ss;
		ss << "0x" << std::hex << (*(size_t*)(printable.pData));
		return lhs += ss.str();
	}

	case PrintableValue::Type::UNSIGNED_INTEGER:
		return lhs += std::to_string((*(PrintableValue::uint_t*)(printable.pData)));

	case PrintableValue::Type::DECIMAL:
		return lhs += std::to_string((*(PrintableValue::dec_t*)(printable.pData)));

	case PrintableValue::Type::TEXT:
		return lhs += (char*)(printable.pData);

	case PrintableValue::Type::UNKNOWN:
		break;
	}
	return lhs;
}
#endif