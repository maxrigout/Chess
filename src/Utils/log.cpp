#include "log.h"

#include <iostream>
#include <string>
#include <cstdarg>
#include <mutex>

static std::mutex mu_console;

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

	std::cout << message << std::endl;
}

void UnpackParams(std::vector<PrintableValue>& values)
{
	return;
}

void PrintPrintables(const char* msg, std::vector<PrintableValue> values)
{
	std::string message = msg;

	for (int i = 0; i < values.size(); ++i)
	{
		std::string token = '{' + std::to_string(i) + '}';
		size_t index = message.find(token);
		if (index == std::string::npos)
			continue;
		std::string before = message.substr(0, index);
		std::string after = message.substr(index + token.length());
		message = before + values[i] + after;
	}

	std::lock_guard<std::mutex> lock(mu_console);
	std::cout << message << std::endl;
}

void PrintAdvanced(const std::string& msg)
{
	std::cout << msg << std::endl;
}

std::string operator+(const std::string& lhs, const PrintableValue& printable)
{
	std::type_index i(printable.info);
	if (i == std::type_index(typeid(bool)))
	{
		return lhs + ((printable.data._bool) ? "true" : "false");
	}
	else if (i == std::type_index(typeid(char)))
	{
		return lhs + printable.data._char;
	}
	else if (i == std::type_index(typeid(short)))
	{
		return lhs + std::to_string(printable.data._short);
	}
	else if (i == std::type_index(typeid(int)))
	{
		return lhs + std::to_string(printable.data._int);
	}
	else if (i == std::type_index(typeid(long)))
	{
		return lhs + std::to_string(printable.data._long);
	}
	else if (i == std::type_index(typeid(float)))
	{
		return lhs + std::to_string(printable.data._float);
	}
	else if (i == std::type_index(typeid(double)))
	{
		return lhs + std::to_string(printable.data._double);
	}
	else if (i == std::type_index(typeid(unsigned char)))
	{
		return lhs + std::to_string(printable.data._uchar);
	}
	else if (i == std::type_index(typeid(unsigned short)))
	{
		return lhs + std::to_string(printable.data._ushort);
	}
	else if (i == std::type_index(typeid(unsigned int)))
	{
		return lhs + std::to_string(printable.data._uint);
	}
	else if (i == std::type_index(typeid(unsigned long)))
	{
		return lhs + std::to_string(printable.data._ulong);
	}
	else if (i == std::type_index(typeid(size_t)))
	{
		return lhs + std::to_string(printable.data._size_t);
	}
	else if (i == std::type_index(typeid(std::string)))
	{
		//return lhs + printable.data._string;
	}
	else if (i == std::type_index(typeid(char*)))
	{
		return lhs + std::string(printable.data._charpointer);
	}
	else if (i == std::type_index(typeid(int8_t)))
	{
		return lhs + std::to_string(printable.data._int8_t);
	}
	else if (i == std::type_index(typeid(uint8_t)))
	{
		return lhs + std::to_string(printable.data._uint8_t);
	}
	else if (i == std::type_index(typeid(int16_t)))
	{
		return lhs + std::to_string(printable.data._int16_t);
	}
	else if (i == std::type_index(typeid(uint16_t)))
	{
		return lhs + std::to_string(printable.data._uint16_t);
	}
	else if (i == std::type_index(typeid(int32_t)))
	{
		return lhs + std::to_string(printable.data._int32_t);
	}
	else if (i == std::type_index(typeid(uint8_t)))
	{
		return lhs + std::to_string(printable.data._uint32_t);
	}
	else if (i == std::type_index(typeid(int64_t)))
	{
		return lhs + std::to_string(printable.data._int64_t);
	}
	else if (i == std::type_index(typeid(uint64_t)))
	{
		return lhs + std::to_string(printable.data._uint64_t);
	}
	return lhs;
}
std::string& operator+=(std::string& lhs, const PrintableValue& printable)
{
	std::type_index i(printable.info);
	if (i == std::type_index(typeid(bool)))
	{
		return lhs += ((printable.data._bool) ? "true" : "false");
	}
	else if (i == std::type_index(typeid(char)))
	{
		return lhs += printable.data._char;
	}
	else if (i == std::type_index(typeid(short)))
	{
		return lhs += std::to_string(printable.data._short);
	}
	else if (i == std::type_index(typeid(int)))
	{
		return lhs += std::to_string(printable.data._int);
	}
	else if (i == std::type_index(typeid(long)))
	{
		return lhs += std::to_string(printable.data._long);
	}
	else if (i == std::type_index(typeid(unsigned char)))
	{
		return lhs += std::to_string(printable.data._uchar);
	}
	else if (i == std::type_index(typeid(unsigned short)))
	{
		return lhs += std::to_string(printable.data._ushort);
	}
	else if (i == std::type_index(typeid(unsigned int)))
	{
		return lhs += std::to_string(printable.data._uint);
	}
	else if (i == std::type_index(typeid(unsigned long)))
	{
		return lhs += std::to_string(printable.data._ulong);
	}
	else if (i == std::type_index(typeid(size_t)))
	{
		return lhs += std::to_string(printable.data._size_t);
	}
	else if (i == std::type_index(typeid(std::string)))
	{
		//return lhs + printable.data._string;
	}
	else if (i == std::type_index(typeid(char*)))
	{
		return lhs += std::string(printable.data._charpointer);
	}
	else if (i == std::type_index(typeid(int8_t)))
	{
		return lhs += std::to_string(printable.data._int8_t);
	}
	else if (i == std::type_index(typeid(uint8_t)))
	{
		return lhs += std::to_string(printable.data._uint8_t);
	}
	else if (i == std::type_index(typeid(int16_t)))
	{
		return lhs += std::to_string(printable.data._int16_t);
	}
	else if (i == std::type_index(typeid(uint16_t)))
	{
		return lhs += std::to_string(printable.data._uint16_t);
	}
	else if (i == std::type_index(typeid(int32_t)))
	{
		return lhs += std::to_string(printable.data._int32_t);
	}
	else if (i == std::type_index(typeid(uint8_t)))
	{
		return lhs += std::to_string(printable.data._uint32_t);
	}
	else if (i == std::type_index(typeid(int64_t)))
	{
		return lhs += std::to_string(printable.data._int64_t);
	}
	else if (i == std::type_index(typeid(uint64_t)))
	{
		return lhs += std::to_string(printable.data._uint64_t);
	}
	return lhs;
}