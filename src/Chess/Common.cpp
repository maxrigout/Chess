#include "Common.h"
#include "Piece.h"

MoveInfo MainInfo;

void Move::PerformMove() const { p->Move(target); }