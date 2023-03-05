#include "King.h"

#include "Rook.h"

#include "Core/Logger.h"

King::King(Board* pBoard, TEAM team, const pt2di& initialBoardPosition)
	: SlowPiece(pBoard, 'K', team, initialBoardPosition, { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, -1}, {0, 1} })
{}

bool King::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
	{
		info.reason = MoveInfo::INVALID_CELL;
		return false;
	}
	vec2di movevect(target - m_boardPosition);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		info.reason = MoveInfo::INVALID_MOVE;
		return false;
	}

	if (m_isFirstMove) // check castle
	{
		if (movevect == vec2di(2, 0))
		{
			Piece* rightTower = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(3, 0));
			if (rightTower == nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			Piece* p1 = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(2, 0));
			Piece* p2 = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(1, 0));
			if (rightTower->HasMoved() || p1 != nullptr || p2 != nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			return true;
		}
		else if (movevect == vec2di(-3, 0))
		{
			Piece* leftTower = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(-4, 0));
			if (leftTower == nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			Piece* p1 = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(-3, 0));
			Piece* p2 = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(-2, 0));
			Piece* p3 = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(-1, 0));
			if (leftTower->HasMoved() || p1 != nullptr || p2 != nullptr || p3 != nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			return true;
		}
	}

	for (const auto& m1 : m_moves)
	{
		if (movevect == m1)
		{
			pt2di target = m_boardPosition + m1;
			if (!m_pBoard->IsPositionValid(target))
			{
				info.reason = MoveInfo::INVALID_CELL;
				return false;
			}
			if (m_pBoard->IsCellAttacked(target, m_team))
			{
				info.reason = MoveInfo::CELL_GUARDED;
				return false;
			}
			Piece* piece = m_pBoard->GetPieceAtCell(target);
			if (piece == nullptr)
			{
				info.reason = MoveInfo::NONE;
				return true;
			}
			if (piece->IsSameTeam(m_team))
			{
				info.reason = MoveInfo::ALREADY_OCCUPIED | MoveInfo::SAME_TEAM;
				return false;
			}
			if (!m_pBoard->IsCellAttacked(target, m_team))
			{
				info.reason = MoveInfo::NONE;
				return true;
			}
			break;	
		}
	}

	info.reason = MoveInfo::INVALID_MOVE;
	return false;
}

void King::Move(const pt2di& target)
{
	vec2di movevect(target - m_boardPosition);
	if (m_isFirstMove)
	{
		// right castle
		if (movevect == vec2di(2, 0))
		{
			Piece* rightRook = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(3, 0));
			// if we're testing this move... the rook should be present (ie not nullptr)
			if (rightRook == nullptr)
			{
				LOG_ERROR("\nright rook is nullptr?...\n" + m_pBoard->ToString());
				return;
			}
			if (rightRook->Type() != 'R')
			{
				LOG_ERROR("\ncannot castle at this time... Piece is not a rook!" + m_pBoard->ToString());
				return;
			}
			Piece::Move(target);
			((Rook*)rightRook)->Castle(vec2di{ -2, 0 });
			return;
		}
		// left castle
		else if (movevect == vec2di(-3, 0))
		{
			Piece* leftRook = m_pBoard->GetPieceAtCell(m_boardPosition + vec2di(-4, 0));
			if (leftRook == nullptr)
			{
				LOG_ERROR("\nleft rook is nullptr?...\n" + m_pBoard->ToString());
				return;
			}
			if (leftRook->Type() != 'R')
			{
				LOG_ERROR("\ncannot castle at this time... Piece is not a rook!" + m_pBoard->ToString());
				return;
			}
			Piece::Move(target);
			((Rook*)leftRook)->Castle(vec2di{ 2, 0 });
			return;
		}
	}
	Piece::Move(target);
}

bool King::CanAttack(const pt2di& target) const
{
	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	// if we're checking against the current piece position
	if (target == m_boardPosition)
		return false;

	return IsMoveValid(target);
}

bool King::Check()
{
	return m_pBoard->IsCellAttacked(m_boardPosition, m_team);
}

void King::CalculateAvailableMoves()
{
	if (m_isMovesCalculated)
		return;

	m_availableMoves.clear();
	for (auto& m : m_moves)
	{
		if (IsMoveValid(m_boardPosition + m))
		{
			m_availableMoves.push_back(m_boardPosition + m);
		}
	}

	if (m_isFirstMove) // check for castle
	{

		if (IsMoveValid(m_boardPosition + vec2di(2, 0)))
		{
			m_availableMoves.push_back(m_boardPosition + vec2di(2, 0));
		}
		if (IsMoveValid(m_boardPosition + vec2di(-3, 0)))
		{
			m_availableMoves.push_back(m_boardPosition + vec2di(-3, 0));
		}
	}
	m_isMovesCalculated = true;
}