#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMInvalidPieceChessException : public std::exception
{
public:
	LLMInvalidPieceChessException(const pt2di& location)
	{
		m_msg = std::format("Invalid chess piece selected at ({}, {}) (most likely belongs to the other player)!", location.x, location.y);
		m_location = location;
	}

	// LLMInvalidPieceChessException(std::string_view location)
	// {
	// 	m_msg = std::format("Invalid chess piece selected at {} (most likely belongs to the other player)!", location);
	// }

	virtual ~LLMInvalidPieceChessException() = default;

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