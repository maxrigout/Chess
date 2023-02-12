#include "Pawn.h"
#include "Chess/Cell.h"

Pawn::Pawn(Board* board, TEAM nTeam, point2d<int> p)
	: Piece(board, nTeam, p, 'P', { {0, 0} }, false)
{
	int i = 1;
	switch (team)
	{
	case TEAM::TWO: i = 1; break;
	case TEAM::ONE: i = -1; break;
	default: break;
	}

	vector2d<int> move_front(0, i);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));
	moves = { move_front, move_front2, capture_left, capture_right };

}
bool Pawn::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	vector2d<int> move_vect(target - boardPosition);
	vector2d<int> move_front(moves[0]);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));

	// if we're checking for an out of bounds cell
	if (target.x > pBoard->GetWidth() || target.y > pBoard->GetHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	else if (move_vect == move_front)
	{
		Cell* target_cell = pBoard->GetCell(boardPosition + move_vect);
		if (target_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (move_vect == move_front2 && first_move)
	{
		Cell* target_cell = pBoard->GetCell(boardPosition + move_vect);
		Cell* intermediate_cell = pBoard->GetCell(boardPosition + move_front);
		if (target_cell == nullptr || intermediate_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece() || intermediate_cell->HasPiece())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (move_vect == capture_left || move_vect == capture_right)
	{
		Cell* target_cell = pBoard->GetCell(boardPosition + move_vect);
		if (target_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool Pawn::CanGuard(const point2d<int>& target) const
{
	vector2d<int> move_vect(target - boardPosition);
	vector2d<int> move_front(moves[0]);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));

	// if we're checking for an out of bounds cell
	if (target.x > pBoard->GetWidth() || target.y > pBoard->GetHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	else if (move_vect == move_front || (move_vect == move_front2 && first_move))
	{
		return false;
	}
	else if (move_vect == capture_left || move_vect == capture_right)
	{
		return true;
	}
	return false;
}