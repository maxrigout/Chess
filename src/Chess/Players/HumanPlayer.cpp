#include "HumanPlayer.h"

#include "DebugLogger.h"

#include <iostream>

HumanPlayer::HumanPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
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