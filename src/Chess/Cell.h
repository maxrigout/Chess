#pragma once
#include "Math/vector2d.h"
#include "Math/point2d.h"
#include "Common.h"

class Piece;

class Cell
{
public:
	Cell() = default;
	~Cell() = default;

	bool PlacePiece(Piece *p);
	bool RemovePiece();
	bool CaptureCell();
	bool HasPiece() const { return (piece != nullptr); }
	bool IsEmpty() const { return (piece == nullptr); }
	bool IsSameTeam(TEAM t) const;
	Piece* GetPiece() const { return piece; }
	point2d<int> GetCoordinates() const { return coordinates; }
	bool IsAttacked() { return m_attack != TEAM::NONE; }
	void Attack(TEAM t) { m_attack = t; }
	void ResetAttacked() { m_attack = TEAM::NONE; }

protected:
	point2d<int> coordinates;
	Piece* piece = nullptr;
	TEAM m_attack = TEAM::NONE;

	friend class Board;
};