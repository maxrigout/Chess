#include "SlowPiece.h"

SlowPiece::SlowPiece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves)
	: Piece(pBoard, type, team, initialBoardPosition, moves)
{
}

bool SlowPiece::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
	if (target == m_boardPosition)
		return false;

	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target)) 
		return false;

	vec2di stepdir;
	vec2di movevect(target - m_boardPosition);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
		return false;

	for (const auto& m : m_moves)
	{
		if (movevect == m)
		{
			Cell* target_cell = m_pBoard->GetCell(m_boardPosition + m);
			if (target_cell == nullptr)
				return false;
			if (target_cell->HasPiece() && target_cell->IsSameTeam(m_team))
				return false;

			return true;
		}
	}

	return false;
}

bool SlowPiece::CanGuard(const pt2di& target) const
{
	// if we're checking against the current piece position
	if (target == m_boardPosition)
		return false;

	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	vec2di stepdir;
	vec2di movevect(target - m_boardPosition);

	for (const auto& m : m_moves)
	{
		if (movevect == m)
		{
			Cell* target_cell = m_pBoard->GetCell(m_boardPosition + m);
			return target_cell != nullptr;
		}
	}

	return false;
}

void SlowPiece::GuardCells() const
{
	for (const auto& move : m_moves)
	{
		pt2di target = m_boardPosition + move;
		m_pBoard->GuardCell(target, m_team);
		// Cell* cell = m_pBoard->GetCell(target);
		// if (cell != nullptr && CanGuard(target))
		// 	cell->Guard(m_team);
	}
}

void SlowPiece::CalculateAvailableMoves()
{
	m_availableMoves.clear();
	for (auto& m : m_moves)
	{
		if (IsMoveValid(m_boardPosition + m))
			m_availableMoves.push_back(m_boardPosition + m);
	}
	m_isMovesCalculated = true;
}
