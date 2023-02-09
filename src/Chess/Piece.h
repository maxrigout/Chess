#pragma once
#include <vector>

#include "Chess/Common.h"
#include "Chess/Board.h"
#include "Renderer2D/Renderer2D.h"

#include "Math/vector2d.h"
#include "Math/point2d.h"

extern MoveInfo MainInfo;

class Piece
{
public:
	Piece(Board* game, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 }, char t = 'N', std::vector<vector2d<int>> moves = { {0, 0} }, bool s = false);
	virtual ~Piece() = default;
	virtual bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const;
	virtual void Move(const point2d<int>& target);
	virtual void DrawMoves(const Renderer2D* renderer) const;
	void DrawYourself(const Renderer2D* renderer) const;
	bool IsSameTeam(TEAM t) const { return (team == t); }
	bool HasMoved() const { return !first_move; }
	TEAM Team() const { return team; }
	bool IsCaptured() const { return isCaptured; }
	bool CapturePiece();
	point2d<int> Pos() const { return pos; }
	char Type() const { return piece_type; }
	void GuardCells();
	virtual bool CanGuard(const point2d<int>& target);

protected:
	point2d<int> pos;
	bool move_scaling;
	std::vector<vector2d<int>> moves;
	Board* pBoard;
	TEAM team;
	Color team_color;
	char piece_type;
	bool isCaptured;
	bool first_move;
};