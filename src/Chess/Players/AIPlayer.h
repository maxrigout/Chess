#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>
#include <thread>

class TestClass;

class AIPlayer : public Player
{
public:
	AIPlayer(Board* pBoard, TEAM team);
	virtual ~AIPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

	virtual void SetOpponentPlayer(Player* player) override { m_opponentPlayer = player; }

	std::vector<Move> GetHypotheticalMoves();
	int EvaluateBoard() const;
	std::vector<Move> GetBestMove(const std::vector<Move>& moves);
	int GetPiecePoints(Piece* piece) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* piece) const;
	int GetPieceValue(char type) const;

	void PlayThread();
	int minimax(Board* pBoard, int depth, bool isMaximizingPlayer);
	int alphabeta(Board* pBoard, int depth, int alpha, int beta, bool isMaximizingPlayer);

	std::vector<Move> GetBestMoves2(const std::vector<Move>& moves);
	void TestMove2(Board* pBoard, const Move& move);
	void UndoMove2(Board* pBoard);
	int EvaluateBoard2(Board* pBoard) const;

private:
	int m_boardScore;

	pt2di m_lastMoveStart{-1, -1};
	pt2di m_lastMoveEnd{-2, -2};

	Player* m_opponentPlayer = nullptr;

	std::thread m_playThread;
	bool m_isPlaying = false;

	size_t m_stackSizeAtBeginningOfTurn = 0;

	class DummyPlayer : public Player
	{
	public:
		DummyPlayer(Board* pBoard, TEAM team) : Player(pBoard, team) {}
		virtual ~DummyPlayer() = default;
		virtual void Play(const PlayingContext& context) {}
	};

	Board* m_boardCopy;
	DummyPlayer* m_copyOfThis;
	DummyPlayer* m_copyOfOpponent;

	friend class TestClass;

};