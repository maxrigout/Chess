#pragma once
#include "Chess/Piece.h"

class Knight : public Piece
{
public:
	Knight(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 })
		: Piece(board, nTeam, p, 'H', { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} }, false)
	{}
	virtual ~Knight() = default;
};