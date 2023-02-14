#pragma once
#include "FastPiece.h"

class Queen : public FastPiece
{
public:
	Queen(Board* board, TEAM team, const pt2di& initialBoardPosition)
		: FastPiece(board, 'Q', team, initialBoardPosition, { {1, 1}, {1, -1}, {0, 1}, {1, 0}, {0, -1 }, {-1, 0}, {-1, -1}, {-1, 1} })
	{}
	virtual ~Queen() = default;
};