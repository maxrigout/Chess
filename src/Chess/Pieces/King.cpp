#include "King.h"

#include <iostream>

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
	vector2d<int> movevect(target - m_boardPosition);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		info.reason = MoveInfo::INVALID_MOVE;
		return false;
	}

	if (m_isFirstMove) // check castle
	{
		if (movevect == vector2d<int>(2, 0))
		{
			Piece* rightTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(3, 0))->GetPiece();
			if (rightTower == nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			Cell* c1 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(2, 0));
			Cell* c2 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(1, 0));
			if (rightTower->HasMoved() || !c1->IsEmpty() || !c2->IsEmpty())
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			return true;
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			Piece* leftTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-4, 0))->GetPiece();
			if (leftTower == nullptr)
			{
				info.reason = MoveInfo::INVALID_MOVE;
				return false;
			}
			Cell* c1 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-3, 0));
			Cell* c2 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-2, 0));
			Cell* c3 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-1, 0));
			if (leftTower->HasMoved() || !c1->IsEmpty() || !c2->IsEmpty() || !c3->IsEmpty())
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
			Cell* target_cell = m_pBoard->GetCell(m_boardPosition + m1);
			if (target_cell == nullptr)
			{
				info.reason = MoveInfo::INVALID_CELL;
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(m_team))
			{
				info.reason = MoveInfo::ALREADY_OCCUPIED | MoveInfo::SAME_TEAM;
				return false;
			}
			else if (target_cell->HasPiece() && !target_cell->IsSameTeam(m_team) && target_cell->IsAttacked())
			{
				info.reason = MoveInfo::CELL_GUARDED;
				return false;
			}
			else if (!target_cell->IsAttacked())
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

// void King::DrawMoves(const Renderer2D* renderer) const
// {
// 	int padx = 2;
// 	int pady = 2;

// 	for (const auto& m : m_moves)
// 	{
// 		if (IsMoveValid(m_boardPosition + m))
// 		{
// 			m_pBoard->HighlightCell(renderer, m_boardPosition + m, {padx, pady}, DARK_GREEN);
// 		}
// 	}

// 	if (m_isFirstMove) // check for castle
// 	{

// 		if (IsMoveValid(m_boardPosition + vector2d<int>(2, 0)))
// 		{
// 			m_pBoard->HighlightCell(renderer, m_boardPosition + vector2d<int>(2, 0), {padx, pady}, DARK_GREEN);
// 		}
// 		if (IsMoveValid(m_boardPosition + vector2d<int>(-3, 0)))
// 		{
// 			m_pBoard->HighlightCell(renderer, m_boardPosition + vector2d<int>(-3, 0), {padx, pady}, DARK_GREEN);
// 		}




// 		Piece* rightTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(3, 0))->GetPiece();
// 		if (rightTower != nullptr)
// 		{
// 			Cell* c1 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(2, 0));
// 			Cell* c2 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(1, 0));
// 			if (!rightTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty())
// 			{
// 				m_pBoard->HighlightCell(renderer, m_boardPosition + vector2d<int>(2, 0), {padx, pady}, DARK_GREEN);
// 			}
// 		}
// 		Piece* leftTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-4, 0))->GetPiece();
// 		if (leftTower != nullptr)
// 		{
// 			Cell* c1 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-3, 0));
// 			Cell* c2 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-2, 0));
// 			Cell* c3 = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-1, 0));
// 			if (!leftTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty() && c3->IsEmpty())
// 			{
// 				m_pBoard->HighlightCell(renderer, m_boardPosition + vector2d<int>(-3, 0), {padx, pady}, DARK_GREEN);
// 			}
// 		}
// 	}
// }

void King::Move(const pt2di& target)
{
	Cell* oldcell = m_pBoard->GetCell(m_boardPosition);
	Cell* newcell = m_pBoard->GetCell(target);
	vector2d<int> movevect(target - m_boardPosition);
	if (m_isFirstMove)
	{
		m_isFirstMove = false;
		if (movevect == vector2d<int>(2, 0))
		{
			Piece* rightTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(3, 0))->GetPiece();
			rightTower->Move(rightTower->Pos() + vector2d<int>(-2, 0));
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			Piece* leftTower = m_pBoard->GetCell(m_boardPosition + vector2d<int>(-4, 0))->GetPiece();
			leftTower->Move(leftTower->Pos() + vector2d<int>(2, 0));
		}
	}
	if (oldcell != nullptr && newcell != nullptr)
	{
		m_boardPosition = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		std::cout << "Moved " << m_pieceType << std::endl;
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
	return m_pBoard->GetCell(m_boardPosition)->IsAttacked();
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

		if (IsMoveValid(m_boardPosition + vector2d<int>(2, 0)))
		{
			m_availableMoves.push_back(m_boardPosition + vector2d<int>(2, 0));
		}
		if (IsMoveValid(m_boardPosition + vector2d<int>(-3, 0)))
		{
			m_availableMoves.push_back(m_boardPosition + vector2d<int>(-3, 0));
		}
	}
	m_isMovesCalculated = true;
}