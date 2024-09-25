#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMDeserializationChessException : public std::exception
{
public:
	LLMDeserializationChessException(std::string_view data)
	{
		m_msg = std::format("unable to deserialize response: ", data);
		m_data = data;
	}

	virtual ~LLMDeserializationChessException() = default;

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}

	const std::string& getData() const
	{
		return m_data;
	}

private:
	std::string m_msg;
	std::string m_data;
};