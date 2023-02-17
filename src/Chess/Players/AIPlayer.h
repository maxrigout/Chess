#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>
#include <stack>

class AIPlayer : public Player
{
public:
	AIPlayer(Board* board, TEAM t, int king_row);
	virtual ~AIPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

private:
	std::vector<Move> GetHypotheticalMoves();
	int EvaluateBoard() const;
	std::vector<Move> GetBestMove(const std::vector<Move>& moves);
	int GetPiecePoints(Piece* p) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* p) const;
	int GetPieceValue(char type) const;

	void TestMove2(const Move& move);
	void UndoMove2();
	int EvaluateBoard2() const;

	int m_boardScore;

	pt2di m_lastMoveStart{};
	pt2di m_lastMoveEnd{};

	std::stack<Move> m_movesStack;
};