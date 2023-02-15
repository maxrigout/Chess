#pragma once
#include "Chess/Piece.h"

class FastPiece : public Piece
{
public:
	FastPiece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves);
	virtual ~FastPiece() = default;

	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const override;
	virtual bool CanAttack(const pt2di& target) const override;
	virtual void AttackCells() const override;
	virtual void CalculateAvailableMoves() override;

private:
	bool IsCellReachable(const pt2di& coords) const;
	bool IsMoveValid2(const pt2di& target, MoveInfo& info) const;
};