#include "King.h"

#include <iostream>

King::King(Board* board, TEAM nTeam, point2d<int> p)
	: Piece(board, nTeam, p, 'K', { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, -1}, {0, 1} }, false)
{}

bool King::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	// if we're checking for an out of bounds cell
	if (target.x > pBoard->GetWidth() || target.y > pBoard->GetHeight() || target.x < 0 || target.y < 0)
	{
		info.reason = MoveInfo::INVALID_CELL;
		return false;
	}
	vector2d<int> movevect(target - pos);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		info.reason = MoveInfo::INVALID_MOVE;
		return false;
	}

	if (first_move) // check castle
	{
		if (movevect == vector2d<int>(2, 0))
		{
			Piece* rightTower = pBoard->GetCell(pos + vector2d<int>(3, 0))->GetPiece();
			if (rightTower != nullptr)
			{
				Cell* c1 = pBoard->GetCell(pos + vector2d<int>(2, 0));
				Cell* c2 = pBoard->GetCell(pos + vector2d<int>(1, 0));
				if (!rightTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty())
				{
					return true;
				}
			}
			info.reason = MoveInfo::INVALID_MOVE;
			return false;
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			Piece* leftTower = pBoard->GetCell(pos + vector2d<int>(-4, 0))->GetPiece();
			if (leftTower != nullptr)
			{
				Cell* c1 = pBoard->GetCell(pos + vector2d<int>(-3, 0));
				Cell* c2 = pBoard->GetCell(pos + vector2d<int>(-2, 0));
				Cell* c3 = pBoard->GetCell(pos + vector2d<int>(-1, 0));
				if (!leftTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty() && c3->IsEmpty())
				{
					return true;
				}
			}
			info.reason = MoveInfo::INVALID_MOVE;
			return false;
		}
	}

	for (const auto& m1 : moves)
	{
		if (movevect == m1)
		{
			Cell* target_cell = pBoard->GetCell(pos + m1);
			if (target_cell == nullptr)
			{
				info.reason = MoveInfo::INVALID_CELL;
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
			{
				info.reason = MoveInfo::ALREADY_OCCUPIED | MoveInfo::SAME_TEAM;
				return false;
			}
			else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team) && target_cell->IsGuarded())
			{
				info.reason = MoveInfo::CELL_GUARDED;
				return false;
			}
			else if (!target_cell->IsGuarded())
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
void King::DrawMoves(const Renderer2D* renderer) const
{
	int padx = 2;
	int pady = 2;

	for (const auto& m : moves)
	{
		if (IsMoveValid(pos + m))
		{
			pBoard->HighlightCell(renderer, pos + m, {padx, pady}, DARK_GREEN);
		}
	}

	if (first_move) // check for castle
	{

		if (IsMoveValid(pos + vector2d<int>(2, 0)))
		{
			pBoard->HighlightCell(renderer, pos + vector2d<int>(2, 0), {padx, pady}, DARK_GREEN);
		}
		if (IsMoveValid(pos + vector2d<int>(-3, 0)))
		{
			pBoard->HighlightCell(renderer, pos + vector2d<int>(-3, 0), {padx, pady}, DARK_GREEN);
		}




		Piece* rightTower = pBoard->GetCell(pos + vector2d<int>(3, 0))->GetPiece();
		if (rightTower != nullptr)
		{
			Cell* c1 = pBoard->GetCell(pos + vector2d<int>(2, 0));
			Cell* c2 = pBoard->GetCell(pos + vector2d<int>(1, 0));
			if (!rightTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty())
			{
				pBoard->HighlightCell(renderer, pos + vector2d<int>(2, 0), {padx, pady}, DARK_GREEN);
			}
		}
		Piece* leftTower = pBoard->GetCell(pos + vector2d<int>(-4, 0))->GetPiece();
		if (leftTower != nullptr)
		{
			Cell* c1 = pBoard->GetCell(pos + vector2d<int>(-3, 0));
			Cell* c2 = pBoard->GetCell(pos + vector2d<int>(-2, 0));
			Cell* c3 = pBoard->GetCell(pos + vector2d<int>(-1, 0));
			if (!leftTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty() && c3->IsEmpty())
			{
				pBoard->HighlightCell(renderer, pos + vector2d<int>(-3, 0), {padx, pady}, DARK_GREEN);
			}
		}
	}
}
void King::Move(const point2d<int>& target)
{
	Cell* oldcell = pBoard->GetCell(pos);
	Cell* newcell = pBoard->GetCell(target);
	vector2d<int> movevect(target - pos);
	if (first_move)
	{
		first_move = false;
		if (movevect == vector2d<int>(2, 0))
		{
			Piece* rightTower = pBoard->GetCell(pos + vector2d<int>(3, 0))->GetPiece();
			rightTower->Move(rightTower->Pos() + vector2d<int>(-2, 0));
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			Piece* leftTower = pBoard->GetCell(pos + vector2d<int>(-4, 0))->GetPiece();
			leftTower->Move(leftTower->Pos() + vector2d<int>(2, 0));
		}
	}
	if (oldcell != nullptr && newcell != nullptr)
	{
		pos = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		std::cout << "Moved " << piece_type << std::endl;
	}
}
bool King::CanGuard(const point2d<int>& target) const
{
	// if we're checking for an out of bounds cell
	if (target.x > pBoard->GetWidth() || target.y > pBoard->GetHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	vector2d<int> movevect(target - pos);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	for (const auto& m1 : moves)
	{
		if (movevect == m1)
		{
			Cell* target_cell = pBoard->GetCell(pos + m1);
			if (target_cell == nullptr)
			{
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
			{
				return true;
			}
			break;
		}
	}
	return false;
}
bool King::Check()
{
	return pBoard->GetCell(pos)->IsGuarded();
}
bool King::CheckMate()
{
	if (Check())
	{
		for (const auto& m : moves)
		{
			if (IsMoveValid(pos+m) && !pBoard->GetCell(pos + m)->IsGuarded())
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

void King::CalculateMoves()
{
	availableMoves.clear();
	for (auto& m : moves)
	{
		if (IsMoveValid(pos + m))
		{
			availableMoves.push_back(pos + m);
		}
	}

	if (first_move) // check for castle
	{

		if (IsMoveValid(pos + vector2d<int>(2, 0)))
		{
			availableMoves.push_back(pos + vector2d<int>(2, 0));
		}
		if (IsMoveValid(pos + vector2d<int>(-3, 0)))
		{
			availableMoves.push_back(pos + vector2d<int>(-3, 0));
		}
	}
	moves_calculated = true;
}