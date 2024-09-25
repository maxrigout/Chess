#pragma once

#include <exception>
#include <format>
#include <string>
#include <string_view>

#include "Math/point2d.h"

class LLMInvalidCellChessException : public std::exception
{
public:
	LLMInvalidCellChessException(std::string_view cellType, const pt2di& cellCoordinates)
	{
		m_msg = std::format("({}, {}) is not a valid cell (type: {})", cellCoordinates.x, cellCoordinates.y, cellType);
		m_cellType = cellType;
		m_cellCoordinates = cellCoordinates;
	}

	virtual ~LLMInvalidCellChessException() = default;

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}

	const std::string& getCellType() const
	{
		return m_cellType;
	}

	const pt2di& getCoordinates() const
	{
		return m_cellCoordinates;
	}

private:
	std::string m_msg;
	std::string m_cellType;
	pt2di m_cellCoordinates;
};