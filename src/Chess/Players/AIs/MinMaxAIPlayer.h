#pragma once
#include "Chess/Players/AIPlayer.h"

class MinMaxAIPlayer : public AIPlayer
{
public:
	MinMaxAIPlayer(Board* pBoard, TEAM team, int searchDepth, int processingTimeout);
	~MinMaxAIPlayer() = default;

	virtual void PlayThread();

	std::vector<Move> GetBestMoves(const std::vector<Move>& moves);
	int minimax(Board* pBoard, int depth, bool isMaximizingPlayer);

	int EvaluateBoard() const;
	int GetPiecePoints(Piece* piece) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* piece) const;
	int GetPieceValue(char type) const;
	int EvaluateBoard(Board* pBoard) const;

private:
	int m_searchDepth;
};