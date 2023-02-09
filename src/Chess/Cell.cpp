#include "Cell.h"
#include "Chess/Piece.h"

bool Cell::PlacePiece(Piece* p)
{
	if (piece != nullptr)
		return false;

	piece = p;
	return true;
}
bool Cell::RemovePiece()
{
	if (piece == nullptr)
		return false;

	piece = nullptr;
	return true;
}
bool Cell::IsSameTeam(TEAM t) const
{
	if (piece == nullptr)
		return true;

	return (piece->IsSameTeam(t));
}
bool Cell::CaptureCell()
{
	if (piece == nullptr)
		return false;

	piece->CapturePiece();
	piece = nullptr;
	return true;
}