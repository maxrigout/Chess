#pragma once
#include "Math/vector2d.h"
#include "Math/point2d.h"
#include "Common.h"

class Piece;

class Cell
{
public:
	Cell() = default;
	Cell(const Cell& other);
	~Cell() = default;

	// void PlacePiece(Piece *p);
	// void RemovePiece();
	// void CaptureCell();
	// bool HasPiece() const { return (m_piece != nullptr); }
	// bool IsEmpty() const { return (m_piece == nullptr); }
	// bool IsSameTeam(TEAM t) const;
	// Piece* GetPiece() const { return m_piece; }
	const pt2di& GetCoordinates() const { return m_coordinates; }
	// bool IsAttacked() const { return m_attackedBy != TEAM::NONE; }
	void Attack(TEAM t) { m_attackedBy = t; }
	void ResetAttacked() { m_attackedBy = TEAM::NONE; }

protected:
	pt2di m_coordinates;
	Piece* m_piece = nullptr;
	TEAM m_attackedBy = TEAM::NONE;

	friend class Board;
};
