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
	Piece(Board* game, TEAM nTeam = TEAM::NONE, pt2di p = { -1, -1 }, char t = 'N', const std::vector<vec2di>& moves = { {0, 0} }, bool s = false);
	virtual ~Piece() = default;
	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const;
	virtual void CalculateMoves();
	virtual void Move(const pt2di& target);
	virtual void DrawMoves(const Renderer2D* renderer) const;
	void UpdateMovement(float dt);
	void DrawYourself(const Renderer2D* renderer) const;
	bool IsSameTeam(TEAM t) const { return (team == t); }
	bool HasMoved() const { return !first_move; }
	TEAM Team() const { return team; }
	bool IsCaptured() const { return isCaptured; }
	bool CapturePiece();
	pt2di Pos() const { return boardPosition; }
	char Type() const { return piece_type; }
	void GuardCells();
	virtual bool CanGuard(const pt2di& target) const;
	int GetMod(TEAM team) const { return ((IsSameTeam(team)) ? 1 : -1); }
	const std::vector<pt2di>& GetAvailableMoves();
	// const std::vector<pt2di>& GetAvailableMoves() const { return availableMoves; }
	// const std::vector<pt2di>& GetLegalMoves() { return legal_moves; }
	// void AddLegalMove(pt2di m) { legal_moves.push_back(m); }
	// void ResetLegalMoves() { legal_moves.clear(); }
	// bool HaveMovesBeenCalculated() const { return moves_calculated; }

private:
	void GuardCellsUnscaled() const;
	void GuardCellsScaled() const;

protected:
	pt2di boardPosition;
	bool move_scaling;
	std::vector<vec2di> moves;
	std::vector<pt2di> availableMoves;
	// std::vector<pt2di> legal_moves;
	Board* pBoard;
	TEAM team;
	Color team_color;
	char piece_type;
	bool isCaptured;
	bool first_move;
	bool moves_calculated;

	pt2df m_screenPosition;
	pt2df m_targetScreenPosition;
	float m_speed = 1.0f;
};

#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"