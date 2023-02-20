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
	Piece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves);
	virtual ~Piece() = default;
	virtual bool IsMoveValid(const pt2di& target, MoveInfo& info = MainInfo) const = 0;
	virtual void CalculateAvailableMoves() = 0;
	virtual void Move(const pt2di& target);
	virtual void AttackCells() const = 0;
	virtual bool CanAttack(const pt2di& target) const = 0;
	void UpdateMovement(float dt);
	void DrawMoves(const Renderer2D* renderer) const;
	void DrawYourself(const Renderer2D* renderer) const;
	TEAM Team() const { return m_team; }
	bool IsSameTeam(TEAM t) const { return (m_team == t); }
	bool HasMoved() const { return !m_isFirstMove; }
	bool IsCaptured() const { return m_isCaptured; }
	void GetCaptured(const pt2di& benchPosition);
	void ResetCaptured();
	const pt2di& Pos() const { return m_boardPosition; }
	char Type() const { return m_pieceType; }
	int GetMod(TEAM team) const { return ((IsSameTeam(team)) ? 1 : -1); }
	const std::vector<pt2di>& GetAvailableMoves();
	void ResetAvailableMoves();
	void AddAvailableMove(const pt2di& move); // used by the player to control which moves are available to the piece
	int GetId() const { return m_id; }
	bool IsFirstMove() { return m_isFirstMove; }
	void ReseFirstMove() { m_isFirstMove = true; }

private:

protected:
	pt2di m_boardPosition;
	std::vector<vec2di> m_moves;
	std::vector<pt2di> m_availableMoves;
	Board* m_pBoard;
	TEAM m_team;
	Color m_teamColor;
	char m_pieceType;
	bool m_isCaptured;
	bool m_isFirstMove;
	bool m_isMovesCalculated;
	int m_id;

	pt2df m_screenPosition;
	pt2df m_targetScreenPosition;
	float m_speed = 15.0f;
};