#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>
#include <stack>
#include <thread>

class AIPlayer : public Player
{
public:
	AIPlayer(Board* pBoard, TEAM team);
	virtual ~AIPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

	void SetOpponentPlayer(Player* player) { m_opponentPlayer = player; }

private:
	std::vector<Move> GetHypotheticalMoves();
	int EvaluateBoard() const;
	std::vector<Move> GetBestMove(const std::vector<Move>& moves);
	int GetPiecePoints(Piece* piece) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* piece) const;
	int GetPieceValue(char type) const;

	void PlayThread();
	int minimax(int depth, bool isMaximizingPlayer);
	int alphabeta(int depth, int alpha, int beta, bool isMaximizingPlayer);

	std::vector<Move> GetBestMoves2(const std::vector<Move>& moves);
	void TestMove2(const Move& move);
	void UndoMove2();
	int EvaluateBoard2() const;

	int m_boardScore;

	pt2di m_lastMoveStart{-1, -1};
	pt2di m_lastMoveEnd{-2, -2};

	std::stack<Move> m_movesStack;

	Player* m_opponentPlayer = nullptr;

	std::thread m_playThread;
	bool m_isPlaying = false;
};