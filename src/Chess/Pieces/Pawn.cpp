#include "Pawn.h"
#include "Chess/Cell.h"

#include <iostream>

Pawn::Pawn(Board* pBoard, TEAM team, const pt2di& initialBoardPosition, PAWN_DIRECTION direction)
	: SlowPiece(pBoard, 'P', team, initialBoardPosition, { {0, 0} })
{
	// determine the direction the pawn must go
	int i = 1;
	switch (direction)
	{
	case PAWN_DIRECTION::DOWN: i = 1; break;
	case PAWN_DIRECTION::UP: i = -1; break;
	}

	vec2di move_front(0, i);
	vec2di move_front2(move_front + move_front);
	vec2di capture_left(move_front + vec2di(-1, 0));
	vec2di capture_right(move_front + vec2di(1, 0));
	m_moves = { move_front, move_front2, capture_left, capture_right };

}
bool Pawn::IsMoveValid(const pt2di& target, MoveInfo& info) const
{
	if (target == m_boardPosition)
		return false;

	vec2di move_vect(target - m_boardPosition);

	vec2di move_front(m_moves[0]);
	vec2di move_front2(move_front + move_front);
	vec2di capture_left(move_front + vec2di(-1, 0));
	vec2di capture_right(move_front + vec2di(1, 0));

	// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	// checking for 1 cell in front
	if (move_vect == move_front)
	{
		Cell* target_cell = m_pBoard->GetCell(m_boardPosition + move_vect);
		return !target_cell->HasPiece();
	}
	// if we're moving 2 spaces forward, we need to check for both cells in front
	if (m_isFirstMove && move_vect == move_front2)
	{
		Cell* intermediate_cell = m_pBoard->GetCell(m_boardPosition + move_front);
		Cell* target_cell = m_pBoard->GetCell(m_boardPosition + move_vect);
		// we didn't check if the position was valid for the intermediate cell
		// if the target cell is valid, then most likely, the intermediate cell will be valid as well
		if (intermediate_cell == nullptr)
			return false;
		// if either cell is occupied, then not valid
		return !target_cell->HasPiece() && !intermediate_cell->HasPiece();
	}
	// if we're capturing a piece
	if (move_vect == capture_left || move_vect == capture_right)
	{
		Cell* target_cell = m_pBoard->GetCell(m_boardPosition + move_vect);
		return target_cell->HasPiece() && !target_cell->IsSameTeam(m_team);
	}

	// TODO there's one more move the pawn can do... need to look at the rules
	return false;
}

bool Pawn::CanGuard(const pt2di& target) const
{
	vec2di move_vect(target - m_boardPosition);
	const vec2di& move_front = m_moves[0];
	vec2di capture_left(move_front + vec2di(-1, 0));
	vec2di capture_right(move_front + vec2di(1, 0));

		// if we're checking for an out of bounds cell
	if (!m_pBoard->IsPositionValid(target))
		return false;

	return move_vect != capture_left && move_vect != capture_right;
}

void Pawn::GuardCells() const
{
	const vec2di& move_front = m_moves[0];
	vec2di capture_left_dir(move_front + vec2di(-1, 0));
	vec2di capture_right_dir(move_front + vec2di(1, 0));
	pt2di left = m_boardPosition + capture_left_dir;
	pt2di right = m_boardPosition + capture_right_dir;
	m_pBoard->GuardCell(left, m_team);
	m_pBoard->GuardCell(right, m_team);
}