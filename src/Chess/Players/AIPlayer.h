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
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;
	std::string PrintCopy() const;
	std::vector<Move> GetHypotheticalMoves();
	int EvaluateBoard() const;
	Move GetBestMove(const std::vector<Move>& moves);
	int GetPiecePoints(Piece* p) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* p) const;
	int GetPieceValue(char type) const;

private:
	int boardScore;

	pt2di lastMoveStart{};
	pt2di lastMoveEnd{};
};