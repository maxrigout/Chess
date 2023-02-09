#pragma once
#include "Chess/Piece.h"

class Queen : public Piece
{
public:
	Queen(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 })
		: Piece(board, nTeam, p, 'Q', { {1, 1}, {1, -1}, {0, 1}, {1, 0}, {0, -1 }, {-1, 0}, {-1, -1}, {-1, 1} }, true)
	{}
	virtual ~Queen() = default;
};