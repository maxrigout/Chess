#pragma once
#include "FastPiece.h"

class Rook : public FastPiece
{
public:
	Rook(Board* pBoard, TEAM team, const pt2di& initialBoardPosition)
		: FastPiece(pBoard, 'R', team, initialBoardPosition, { { 0, 1 } , {1, 0}, {0, -1 }, {-1, 0} })
	{}
	virtual ~Rook() = default;
};