#include "Cell.h"
#include "Chess/Piece.h"

Cell::Cell(const Cell& other)
{
	m_coordinates = other.m_coordinates;
	m_attackedBy = other.m_attackedBy;
}
