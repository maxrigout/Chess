#include "Cell.h"
#include "Chess/Piece.h"

void Cell::PlacePiece(Piece* p)
{
	piece = p;
}
void Cell::RemovePiece()
{
	piece = nullptr;
}
bool Cell::IsSameTeam(TEAM t) const
{
	if (piece == nullptr)
		return true;

	return (piece->IsSameTeam(t));
}
void Cell::CaptureCell()
{
	if (piece == nullptr)
		return;

	piece->CapturePiece();
	piece = nullptr;
}