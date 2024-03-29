#include "FastPiece.h"

FastPiece::FastPiece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves)
	: Piece(pBoard, type, team, initialBoardPosition, moves)
{

}

bool FastPiece::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
    return std::any_of(m_availableMoves.begin(), m_availableMoves.end(), [&](const pt2di& move) {
       return (move == target);
    });
}

bool FastPiece::CanAttack(const pt2di& target) const
{
	// if we're checking for an out-of-bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	vec2di stepDir;
	vec2di moveVector(target - m_boardPosition);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (moveVector.x == 0 && moveVector.y == 0)
		return false;

	for (const auto& m : m_moves)
	{
		if (moveVector.IsPositiveMultiple(m))
		{
			if (m.x != 0)
			{
				nSteps = moveVector.x / m.x;
			}
			else
			{
				nSteps = moveVector.y / m.y;
			}
            stepDir = m;
			found_dir = true;
			break;
		}
	}

	// check each cell between current pos and target pos
	if (found_dir) {
		std::vector<pt2di> path;
		bool blocked = false;
		path.resize(nSteps);

		for (int i = 1; i < nSteps + 1; ++i)
		{
			path[i - 1] = m_boardPosition + (stepDir * i);
		}
		for (const auto& tile : path)
		{
			if (!m_pBoard->IsPositionValid(tile))
				return false;
			
			Piece* piece = m_pBoard->GetPieceAtCell(tile);
			if (piece != nullptr && !piece->IsSameTeam(m_team) && piece->Type() != 'K')
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
			if (m_pBoard->GetPieceAtCell(target_cell) != nullptr)
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
			if (m_pBoard->GetPieceAtCell(targetCoords) != nullptr)
				break;
		}
	}

	m_isMovesCalculated = true;
}

bool FastPiece::IsCellReachable(const pt2di& coords) const
{
	if (!m_pBoard->IsPositionValid(coords))
		return false;
	
	Piece* piece = m_pBoard->GetPieceAtCell(coords);
	if (piece != nullptr)
		return !piece->IsSameTeam(m_team);
	return true;
}
