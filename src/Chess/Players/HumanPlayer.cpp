#include "HumanPlayer.h"

#include "Logger.h"

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

	// attempt to move the piece
	MoveInfo info = MoveSelectedPiece(context.selectedCellPosition);
	if (info.reason == MoveInfo::NONE)
	{
		EndTurn(); // end the current player's turn and allow the other player to play
		return;
	}
	SelectPiece(context.selectedCellPosition);
}