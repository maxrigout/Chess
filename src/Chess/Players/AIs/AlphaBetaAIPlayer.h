#pragma once
#include "Chess/Players/AIPlayer.h"

class AlphaBetaAIPlayer : public AIPlayer
{
public:
	AlphaBetaAIPlayer(Board* pBoard, TEAM team, int searchDepth, int processingTimeout);
	~AlphaBetaAIPlayer() {}

	virtual void PlayThread();

	std::vector<Move> GetBestMoves(const std::vector<Move>& moves);
	int alphabeta(Board* pBoard,  int depth, int alpha, int beta, bool isMaximizingPlayer);

	int EvaluateBoard() const;
	int GetPiecePoints(Piece* piece) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* piece) const;
	int GetPieceValue(char type) const;
	int EvaluateBoard(Board* pBoard) const;

private:
	int m_searchDepth;
};