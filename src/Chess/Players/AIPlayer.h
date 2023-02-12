#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>

class AIPlayer : public Player
{
public:
	AIPlayer(Board* board, TEAM t, int king_row);
	virtual ~AIPlayer();
	virtual void Play(const Context& context) override;
	std::string PrintCopy();
	std::vector<Move> GetHypotheticalMoves();
	int EvaluateBoard();
	Move GetBestMove(const std::vector<Move>& moves);
	int GetPiecePoints(Piece* p);
	int GetPiecePoints(char type);
	int GetPieceValue(Piece* p);
	int GetPieceValue(char type);
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

private:
	int boardScore;

	pt2di lastMoveStart{};
	pt2di lastMoveEnd{};
};