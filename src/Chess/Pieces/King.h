#pragma once
#include "SlowPiece.h"

class King : public SlowPiece
{
public:
	King(Board* pBoard, TEAM team, const pt2di& initialBoardPosition);
	virtual ~King() = default;
	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const override;
	// virtual void DrawMoves(const Renderer2D* renderer) const override;
	virtual void Move(const pt2di& target) override;
	virtual bool CanAttack(const pt2di& target) const override;
	virtual void CalculateAvailableMoves() override;
	
	bool Check();
	bool CheckMate();
};