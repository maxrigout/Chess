#pragma once
#include "Chess/Piece.h"

class SlowPiece : public Piece
{
public:
	SlowPiece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves);
	virtual ~SlowPiece() = default;

	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const override;
	virtual bool CanAttack(const pt2di& target) const override;
	virtual void AttackCells() const override;
	virtual void CalculateAvailableMoves() override;
};
