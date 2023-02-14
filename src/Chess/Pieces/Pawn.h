#pragma once
#include "Chess/Pieces/SlowPiece.h"

enum class PAWN_DIRECTION {
	UP,
	DOWN,
};

class Pawn : public SlowPiece
{
public:
	Pawn(Board* pBoard, TEAM team, const pt2di& initialBoardPosition, PAWN_DIRECTION direction);
	virtual ~Pawn() = default;
	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const override;
	virtual bool CanGuard(const pt2di& target) const override;
	virtual void GuardCells() const override;

};