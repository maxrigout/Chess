#include "Piece.h"

#include <iostream>
#include <string>

Piece::Piece(Board* board, TEAM nTeam, pt2di p, char t, const std::vector<vec2di>& m, bool s)
{
	first_move = true;
	isCaptured = false;
	pBoard = board; 
	team = nTeam;
	boardPosition = p;
	piece_type = t;
	moves = m;
	move_scaling = s;

	m_screenPosition = pBoard->BoardToWindowCoordinates(boardPosition);
	m_targetScreenPosition = m_screenPosition;

	switch (team)
	{
	case TEAM::ONE: team_color = DARK_GREY; break;
	case TEAM::TWO: team_color = DARK_YELLOW; break;
	default: team_color = GREEN; break;
	}
}

bool Piece::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
	/* * STEP 1: CHECK TO SEE IF FIRST MOVE (FOR KING AND PAWN)
	   * STEP 2: CHECK TO SEE IF THERE IS A PIECE AT THAT LOCATION
	   * STEP 2.1: CHECK TO SEE IF IT'S AN ENEMY PIECE (YES CONTINUE, NO RETURN FALSE)
	   * STEP 3: CHECK TO SEE IF YOU CAN GET TO THE TARGET POSITION
	*/

	// if we're checking for an out of bounds cell
	if (!pBoard->IsPositionValid(target)) 
		return false;

	vec2di stepdir;
	vec2di movevect(target - boardPosition);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
		return false;

	if (!move_scaling)
	{
		for (const auto& m : moves)
		{
			if (movevect == m)
			{
				Cell* target_cell = pBoard->GetCell(boardPosition + m);
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
		for (const auto& m : moves)
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
				path[i - 1] = boardPosition + (stepdir * i);
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

bool Piece::CanGuard(const pt2di& target) const
{
	// if we're checking for an out of bounds cell
	if (!pBoard->IsPositionValid(target))
		return false;

	vec2di stepdir;
	vec2di movevect(target - boardPosition);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
		return false;

	if (!move_scaling)
	{
		for (const auto& m : moves)
		{
			if (movevect == m)
			{
				Cell* target_cell = pBoard->GetCell(boardPosition + m);
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
		for (const auto& m : moves)
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
				path[i - 1] = boardPosition + (stepdir * i);
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

void Piece::GuardCellsUnscaled() const
{
	for (const auto& m : moves)
	{
		Cell* cell = pBoard->GetCell(boardPosition + m);
		if (cell != nullptr && CanGuard(boardPosition + m))
			cell->Guard(team);
	}
}

void Piece::GuardCellsScaled() const
{
	for (const auto& m : moves)
	{
		pt2di target_cell;
		const int maxDistance = std::max(pBoard->GetWidth(), pBoard->GetHeight());
		for (int i = 1; i < maxDistance; ++i)
		{
			target_cell = boardPosition + (m * i);
			if (!CanGuard(target_cell))
				break;
			pBoard->GetCell(target_cell)->Guard(team);
		}
	}
}

void Piece::GuardCells()
{
	if (!move_scaling)
		GuardCellsUnscaled();
	else
		GuardCellsScaled();
	moves_calculated = false;
}

void Piece::UpdateMovement(float dt)
{
	vec2df moveVector = m_targetScreenPosition - m_screenPosition;
	m_screenPosition = m_screenPosition + moveVector * m_speed * dt;
}

void Piece::DrawMoves(const Renderer2D* renderer) const
{
	int padx = 2;
	int pady = 2;

	if (!move_scaling)
	{
		for (auto& m : moves)
		{
			if (IsMoveValid(boardPosition + m)) 
			{
				pBoard->HighlightCell(renderer, boardPosition + m, {padx, pady}, DARK_GREEN);
			}
		}
	}
	else
	{
		const int maxDistance = std::max(pBoard->GetWidth(), pBoard->GetHeight());
		for (auto& m : moves)
		{
			pt2di target_cell;
			for (int i = 1; i < maxDistance; ++i)
			{
				target_cell = boardPosition + (m * i);
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

void Piece::DrawYourself(const Renderer2D* renderer) const
{
	if (!isCaptured)
	{
		vec2di cell = renderer->GetCellDim();
		pt2di center = m_screenPosition + vec2df(cell) / 2;
		renderer->FillCircle(center, cell.w / 3, team_color);
		renderer->DrawText(m_screenPosition, {piece_type, '\0'}, WHITE);
	}
}

void Piece::Move(const pt2di& target)
{
	Cell* oldcell = pBoard->GetCell(boardPosition);
	Cell* newcell = pBoard->GetCell(target);
	if (oldcell != nullptr && newcell != nullptr)
	{
		boardPosition = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		m_targetScreenPosition = pBoard->BoardToWindowCoordinates(target);
		std::cout << "Moved " << piece_type << " from " << pBoard->GetBoardCoordinates(oldcell->GetCoordinates()) << " to " << pBoard->GetBoardCoordinates(newcell->GetCoordinates()) << std::endl;
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

void Piece::CalculateMoves()
{
	availableMoves.clear();
	if (move_scaling)
	{
		for (const vec2di& m : moves)
		{
			pt2di target_cell;
			const int maxDist = 8;
			for (int i = 1; i < maxDist; ++i)
			{
				target_cell = boardPosition + (m * i);
				if (IsMoveValid(target_cell))
				{
					availableMoves.push_back(boardPosition + m * i);
				}
				else
				{
					break;
				}
			}
		}

	}
	else
	{
		for (auto& m : moves)
		{
			if (IsMoveValid(boardPosition + m))
			{
				availableMoves.push_back(boardPosition + m);
			}
		}
	}
	moves_calculated = true;
}

const std::vector<pt2di>& Piece::GetAvailableMoves()
{
	if (!moves_calculated)
		CalculateMoves();
	return availableMoves;
}
