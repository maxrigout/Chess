#include "HumanPlayer.h"

#include "DebugLogger.h"

#include <iostream>

HumanPlayer::HumanPlayer(Board* board, TEAM team, int king_row)
	: Player(board, team, king_row)
{
}

HumanPlayer::~HumanPlayer() = default;

void HumanPlayer::Play(const PlayingContext& context)
{
	if (!IsPieceSelected())
	{
		SelectPiece(context.selectedCellPosition);
		return;
	}

	MoveInfo info = MoveSelectedPiece(context.selectedCellPosition);
	if (info.reason != MoveInfo::NONE)
		SelectPiece(context.selectedCellPosition);
}