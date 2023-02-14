#pragma once
#include "Chess/Pieces/SlowPiece.h"

class Knight : public SlowPiece
{
public:
	Knight(Board* pBoard, TEAM team, const pt2di& initialBoardPosition)
		: SlowPiece(pBoard, 'H', team, initialBoardPosition, { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} })
	{}
	virtual ~Knight() = default;
};