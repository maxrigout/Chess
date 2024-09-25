#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMInvalidMoveChessException : public std::exception
{
public:
	LLMInvalidMoveChessException(std::string_view pieceType, const pt2di start, const pt2di& end)
	{
		m_msg = std::format("invalid move for a {}: ({}, {}) -> ({}, {})", pieceType, start.x, start.y, end.x, end.y);
		m_pieceType = pieceType;
		m_start = start;
		m_end = end;
	}

	virtual ~LLMInvalidMoveChessException() = default;

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}

	const std::string& getPieceType() const
	{
		return m_pieceType;
	}

	const pt2di& getStart() const
	{
		return m_start;
	}

	const pt2di& getEnd() const
	{
		return m_end;
	}

private:
	std::string m_msg;
	std::string m_pieceType;
	pt2di m_start;
	pt2di m_end;
};