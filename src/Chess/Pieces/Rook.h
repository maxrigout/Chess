#pragma once
#include "Chess/Piece.h"

class Rook : public Piece
{
public:
	Rook(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 })
		: Piece(board, nTeam, p, 'R', { { 0, 1 } , {1, 0}, {0, -1 }, {-1, 0} }, true)
	{}
	virtual ~Rook() = default;
};