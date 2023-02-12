#pragma once
#include "Chess/Piece.h"

class King : public Piece
{
public:
	King(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 });
	virtual ~King() = default;
	virtual bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const override;
	virtual void DrawMoves(const Renderer2D* renderer) const override;
	virtual void Move(const point2d<int>& target) override;
	virtual bool CanGuard(const point2d<int>& target) const override;
	bool Check();
	bool CheckMate();
	virtual void CalculateMoves() override;
};