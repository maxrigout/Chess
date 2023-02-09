#pragma once
#include "Chess/Piece.h"

class Pawn : public Piece
{
public:
	Pawn(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 });
	virtual ~Pawn() = default;
	bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const override;
	bool CanGuard(const point2d<int>& target) override;
};