#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMNoPieceChessException : public std::exception
{
public:
	LLMNoPieceChessException(const pt2di& location)
	{
		m_msg = std::format("No piece found at location ({}, {})", location.x, location.y);
		m_location = location;
	}

	// LLMNoPieceChessException(std::string_view location)
	// {
	// 	m_msg = std::format("No piece found at location {}", location);
	// }
	
	virtual ~LLMNoPieceChessException() = default;

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}

	const pt2di& getLocation() const
	{
		return m_location;
	}

private:
	std::string m_msg;
	pt2di m_location;
};