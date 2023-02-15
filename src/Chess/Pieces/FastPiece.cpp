#include "FastPiece.h"

FastPiece::FastPiece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves)
	: Piece(pBoard, type, team, initialBoardPosition, moves)
{

}

bool FastPiece::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
	for (const auto& move : m_availableMoves)
	{
		if (move == target)
			return true;
	}
	return false;
}

bool FastPiece::CanAttack(const pt2di& target) const
{
	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	vec2di stepdir;
	vec2di movevect(target - m_boardPosition);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
		return false;

	for (const auto& m : m_moves)
	{
		if (movevect.IsPositiveMultiple(m))
		{
			if (m.x != 0)
			{
				nSteps = movevect.x / m.x;
			}
			else
			{
				nSteps = movevect.y / m.y;
			}
			stepdir = m;
			found_dir = true;
			break;
		}
	}

	// check each cells between current pos and target pos
	if (found_dir) {
		std::vector<pt2di> path;
		bool blocked = false;
		path.resize(nSteps);

		for (int i = 1; i < nSteps + 1; ++i)
		{
			path[i - 1] = m_boardPosition + (stepdir * i);
		}
		for (auto tile : path)
		{
			Cell* target_cell = m_pBoard->GetCell(tile);
			if (target_cell == nullptr)
			{
				return false;
			}
			else if (target_cell->HasPiece() && !target_cell->IsSameTeam(m_team) && target_cell->GetPiece()->Type() != 'K')
			{
				blocked = true;
			}
			else if (blocked)
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

void FastPiece::AttackCells() const
{
	for (const auto& m : m_moves)
	{
		pt2di target_cell;
		const int maxDistance = std::max(m_pBoard->GetWidth(), m_pBoard->GetHeight());
		for (int i = 1; i < maxDistance; ++i)
		{
			target_cell = m_boardPosition + (m * i);
			if (!CanAttack(target_cell))
				break;
			m_pBoard->AttackCell(target_cell, m_team);
			if (m_pBoard->GetCell(target_cell)->GetPiece() != nullptr)
				break;
		}
	}
}

void FastPiece::CalculateAvailableMoves()
{
	m_availableMoves.clear();

	for (const vec2di& move : m_moves)
	{
		pt2di targetCoords;
		const int maxDistance = std::max(m_pBoard->GetWidth(), m_pBoard->GetHeight());
		for (int i = 1; i < maxDistance; ++i)
		{
			targetCoords = m_boardPosition + (move * i);
			// it's ok to just check if we can go to that cell directly
			// because we're checking all the cells in order
			if (!IsCellReachable(targetCoords))
				break;
			m_availableMoves.push_back(m_boardPosition + move * i);
			// if we hit an enemy piece (ie. we can get there), we can't continue
			if (m_pBoard->GetCell(targetCoords)->GetPiece() != nullptr)
				break;
		}
	}

	m_isMovesCalculated = true;
}

bool FastPiece::IsCellReachable(const pt2di& coords) const
{
	Cell* targetCell = m_pBoard->GetCell(coords);
	if (targetCell == nullptr)
		return false;
	
	if (targetCell->HasPiece())
		return !targetCell->GetPiece()->IsSameTeam(m_team);
	return true;
}

bool FastPiece::IsMoveValid2(const pt2di& target, MoveInfo& info) const
{
	/* * STEP 1: CHECK TO SEE IF FIRST MOVE (FOR KING AND PAWN)
	   * STEP 2: CHECK TO SEE IF THERE IS A PIECE AT THAT LOCATION
	   * STEP 2.1: CHECK TO SEE IF IT'S AN ENEMY PIECE (YES CONTINUE, NO RETURN FALSE)
	   * STEP 3: CHECK TO SEE IF YOU CAN GET TO THE TARGET POSITION
	*/

	if (target == m_boardPosition)
		return false;

	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target)) 
		return false;

	vec2di stepdir;
	vec2di movevect(target - m_boardPosition);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
		return false;

	for (const auto& m : m_moves)
	{
		if (movevect.IsPositiveMultiple(m))
		{
			if (m.x != 0)
			{
				nSteps = movevect.x / m.x;
			}
			else
			{
				nSteps = movevect.y / m.y;
			}
			stepdir = m;
			found_dir = true;
			break;
		}
	}

	// check each cells between current pos and target pos
	if (found_dir)
	{
		std::vector<pt2di> path;
		bool blocked = false;

		path.resize(nSteps);

		for (int i = 1; i < nSteps + 1; ++i)
		{
			path[i - 1] = m_boardPosition + (stepdir * i);
		}
		for (auto tile : path)
		{
			Cell* target_cell = m_pBoard->GetCell(tile);
			if (target_cell == nullptr)
			{
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(m_team))
			{
				return false;
			}
			else if (target_cell->HasPiece() && !target_cell->IsSameTeam(m_team))
			{
				blocked = true;
			}
			else if (blocked)
			{
				return false;
			}
		}
		return true;
	}

	return false;
}
