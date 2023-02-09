#include "Piece.h"

#include <iostream>

Piece::Piece(Board* board, TEAM nTeam, point2d<int> p, char t, std::vector<vector2d<int>> m, bool s)
{ 
	first_move = true;
	isCaptured = false;
	pBoard = board; 
	team = nTeam;
	pos = p;
	piece_type = t;
	moves = m;
	move_scaling = s;

	switch (team)
	{
	case TEAM::ONE: team_color = DARK_GREY; break;
	case TEAM::TWO: team_color = DARK_YELLOW; break;
	default: team_color = GREEN; break;
	}
}
bool Piece::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	/* * STEP 1: CHECK TO SEE IF FIRST MOVE (FOR KING AND PAWN)
	   * STEP 2: CHECK TO SEE IF THERE IS A PIECE AT THAT LOCATION
	   * STEP 2.1: CHECK TO SEE IF IT'S AN ENEMY PIECE (YES CONTINUE, NO RETURN FALSE)
	   * STEP 3: CHECK TO SEE IF YOU CAN GET TO THE TARGET POSITION
	*/

	// if we're checking for an out of bounds cell
	if (target.x > pBoard->GetWidth() || target.y > pBoard->GetWidth() || target.x < 0 || target.y < 0) 
	{ 
		return false; 
	}

	vector2d<int> stepdir;
	point2d<int> step(pos);
	vector2d<int> movevect(target - pos);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	if (!move_scaling)
	{
		for (auto m : moves)
		{
			if (movevect == m)
			{
				Cell* target_cell = pBoard->GetCell(pos + m);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (auto m : moves)
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
			std::vector<point2d<int>> path;
			bool blocked = false;

			path.resize(nSteps);

			for (int i = 1; i < nSteps + 1; ++i)
			{
				path[i - 1] = pos + (stepdir * i);
			}
			for (auto tile : path)
			{
				Cell* target_cell = pBoard->GetCell(tile);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
				{
					return false;
				}
				else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team))
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
	}
	return false;
}
bool Piece::CanGuard(const point2d<int>& target)
{
	// if we're checking for an out of bounds cell
	if (target.x >= pBoard->GetWidth() || target.y >= pBoard->GetWidth() || target.x < 0 || target.y < 0)
	{
		return false;
	}

	vector2d<int> stepdir;
	point2d<int> step(pos);
	vector2d<int> movevect(target - pos);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	if (!move_scaling)
	{
		for (auto m : moves)
		{
			if (movevect == m)
			{
				Cell* target_cell = pBoard->GetCell(pos + m);
				if (target_cell == nullptr)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (auto m : moves)
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
			std::vector<point2d<int>> path;
			bool blocked = false;
			path.resize(nSteps);

			for (int i = 1; i < nSteps + 1; ++i)
			{
				path[i - 1] = pos + (stepdir * i);
			}
			for (auto tile : path)
			{
				Cell* target_cell = pBoard->GetCell(tile);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team) && target_cell->GetPiece()->Type() != 'K')
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
	}
	return false;
}
void Piece::DrawMoves(const Renderer2D* renderer) const
{
	int padx = 2;
	int pady = 2;

	if (!move_scaling)
	{
		for (auto& m : moves)
		{
			if (IsMoveValid(pos + m)) 
			{
				pBoard->HighlightCell(renderer, pos + m, {padx, pady}, DARK_GREEN);
			}
		}
	}
	else
	{
		for (auto& m : moves)
		{
			point2d<int> target_cell;

			for (int i = 1; i < 8; ++i)
			{
				target_cell = pos + (m * i);
				if (IsMoveValid(target_cell))
				{
					pBoard->HighlightCell(renderer, target_cell, {padx, pady}, DARK_GREEN);
				}
				else
				{
					break;
				}
			}
		}
	}
}
void Piece::GuardCells()
{
	if (!move_scaling)
	{
		for (auto& m : moves)
		{
			Cell* cell = pBoard->GetCell(pos + m);
			if (cell != nullptr && CanGuard(pos + m))
			{
				cell->Guard(team);
			}
		}
	}
	else
	{
		for (auto& m : moves)
		{
			point2d<int> target_cell;

			for (int i = 1; i < 8; ++i)
			{
				target_cell = pos + (m * i);
				if (CanGuard(target_cell))
				{
					pBoard->GetCell(target_cell)->Guard(team);
				}
				else
				{
					break;
				}
			}
		}
	}
}
void Piece::DrawYourself(const Renderer2D* renderer) const
{
	// TODO
	if (!isCaptured) {
		point2d<int> center{pos.x * renderer->cellWidth + renderer->cellWidth / 2, pos.y * renderer->cellHeight + renderer->cellHeight / 2};
		renderer->FillCircle(center, renderer->cellWidth / 3, team_color);
		// renderer->renderer->DrawDisk(pos, team_color);
		// renderer->renderer->DrawLabel(pos, { piece_type, '\0' }, WHITE);
	}
}
void Piece::Move(const point2d<int>& target)
{
	Cell* oldcell = pBoard->GetCell(pos);
	Cell* newcell = pBoard->GetCell(target);
	if (oldcell != nullptr && newcell != nullptr)
	{
		pos = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		std::cout << "Moved " << piece_type << std::endl;
	}
	if (first_move)
	{
		first_move = false;
	}
}
bool Piece::CapturePiece()
{
	if (!isCaptured)
	{
		isCaptured = true;
		return true;
	}
	return false;
}