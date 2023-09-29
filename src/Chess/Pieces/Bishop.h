#pragma once
#include "FastPiece.h"

class Bishop : public FastPiece
{
public:
	Bishop(Board* board, TEAM team, const pt2di& initialBoardPosition)
		: FastPiece(board, 'B', team, initialBoardPosition, sMoves)
	{}
	virtual ~Bishop() override = default;

	static const std::vector<vec2di> sMoves;
};
