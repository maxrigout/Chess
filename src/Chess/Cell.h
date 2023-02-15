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

	void PlacePiece(Piece *p);
	void RemovePiece();
	void CaptureCell();
	bool HasPiece() const { return (piece != nullptr); }
	bool IsEmpty() const { return (piece == nullptr); }
	bool IsSameTeam(TEAM t) const;
	Piece* GetPiece() const { return piece; }
	const pt2di& GetCoordinates() const { return coordinates; }
	bool IsAttacked() { return m_attack != TEAM::NONE; }
	void Attack(TEAM t) { m_attack = t; }
	void ResetAttacked() { m_attack = TEAM::NONE; }

protected:
	pt2di coordinates;
	Piece* piece = nullptr;
	TEAM m_attack = TEAM::NONE;

	friend class Board;
};