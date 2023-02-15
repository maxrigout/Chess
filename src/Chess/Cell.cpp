#include "Cell.h"
#include "Chess/Piece.h"

void Cell::PlacePiece(Piece* p)
{
	m_piece = p;
}
void Cell::RemovePiece()
{
	m_piece = nullptr;
}
bool Cell::IsSameTeam(TEAM t) const
{
	if (m_piece == nullptr)
		return true;

	return (m_piece->IsSameTeam(t));
}
void Cell::CaptureCell()
{
	if (m_piece == nullptr)
		return;

	m_piece->CapturePiece();
	m_piece = nullptr;
}