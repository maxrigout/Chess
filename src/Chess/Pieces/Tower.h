#pragma once
#include "Chess/Piece.h"

class Tower : public Piece
{
public:
	Tower(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 })
		: Piece(board, nTeam, p, 'T', { { 0, 1 } , {1, 0}, {0, -1 }, {-1, 0} }, true)
	{}
	virtual ~Tower() = default;
};