#pragma once
#include "Chess/Piece.h"

class Bishop : public Piece
{
public:
	Bishop(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 })
		: Piece(board, nTeam, p, 'B', { { 1, 1 } , {1, -1}, {-1, -1}, {-1, 1} }, true)
	{}
	virtual ~Bishop() = default;
};