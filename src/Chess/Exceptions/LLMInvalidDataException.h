#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMInvalidDataChessException : public std::exception
{
public:
	LLMInvalidDataChessException(std::string_view type, std::string_view data)
	{
		m_msg = std::format("invalid data received (type: {}, data: {})", type, data);
		m_type = type;
		m_data = data;
	}

	virtual ~LLMInvalidDataChessException() = default;

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}

	const std::string& getType() const
	{
		return m_type;
	}

	const std::string& getData() const
	{
		return m_data;
	}

private:
	std::string m_msg;
	std::string m_type;
	std::string m_data;
};