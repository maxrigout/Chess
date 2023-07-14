#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>
#include <thread>
#include <memory>
#include <chrono>

class TestClass;

class AIPlayer : public Player
{
public:
	AIPlayer(Board* pBoard, TEAM team, int searchDepth);
	virtual ~AIPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

	int EvaluateBoard() const;
	int GetPiecePoints(Piece* piece) const;
	int GetPiecePoints(char type) const;
	int GetPieceValue(Piece* piece) const;
	int GetPieceValue(char type) const;

	void PlayThread();
	int minimax(Board* pBoard, int depth, bool isMaximizingPlayer);
	int alphabeta(Board* pBoard,  int depth, int alpha, int beta, bool isMaximizingPlayer);

	std::vector<Move> GetBestMoves(const std::vector<Move>& moves);
	void TestMove(Piece* piece, const Move& move);
	void UndoMove(Board* pBoard);
	int EvaluateBoard(Board* pBoard) const;

private:
	int m_boardScore;

	pt2di m_lastMoveStart{-100, -100};
	pt2di m_lastMoveEnd{-100, -100};

	Player* m_opponentPlayer = nullptr;

	std::thread m_playThread;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_playBegin;
	bool m_isPlaying = false;
	bool m_shouldStopProcessing = false;

	size_t m_stackSizeAtBeginningOfTurn = 0;

	class DummyPlayer : public Player
	{
	public:
		DummyPlayer(Board* pBoard, TEAM team) : Player(pBoard, team) {}
		virtual ~DummyPlayer() = default;
		virtual void Play(const PlayingContext& context) {}
	};

	class MaximizingPlayer : public Player
	{
	public:
		MaximizingPlayer(Board* pBoard, TEAM team) : Player(pBoard, team) {}
		virtual ~MaximizingPlayer() = default;
		virtual void Play(const PlayingContext& context) {}
		void Maximize();
	};

	class MinimizingPlayer : public Player
	{
	public:
		MinimizingPlayer(Board* pBoard, TEAM team) : Player(pBoard, team) {}
		virtual ~MinimizingPlayer() = default;
		virtual void Play(const PlayingContext& context) {}
		void Minimize();
	};

	std::unique_ptr<Board> m_boardCopy;
	std::unique_ptr<MaximizingPlayer> m_copyOfThis;
	std::unique_ptr<MinimizingPlayer> m_copyOfOpponent;

	int m_searchDepth;

	friend class TestClass;

};
