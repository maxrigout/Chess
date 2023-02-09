#pragma once
#include "Chess/Piece.h"

class King : public Piece
{
public:
	King(Board* board, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 });
	virtual ~King() = default;
	bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const override;
	void DrawMoves(const Renderer2D* renderer) const override;
	void Move(const point2d<int>& target) override;
	bool CanGuard(const point2d<int>& target) override;
	bool Check();
	bool CheckMate();
};