#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <vector>

class ChessAI : public Player
{
public:
	ChessAI(Board* board, Player* own_pieces, Player* enemy_pieces);
	void Play();
	void CopyBoard();
	std::string PrintCopy();
	void GetPossibleMoves();
	std::vector<Move> GetHypotheticalMoves();
	char TestMove(const Move& move);
	void UndoMove(const Move& move, char old_cell);
	int EvaluateBoard();
	Move GetBestMove();
	int GetPiecePoints(Piece* p);
	int GetPiecePoints(char type);
	int GetPieceValue(Piece* p);
	int GetPieceValue(char type);
	bool IsCheck();

	TEAM Team() { return team; }

private:
	Board* pBoard;
	char** board_copy;
	Piece*** board_hard_copy;
	int boardScore;
	bool ready;
	TEAM team;
	std::vector<Move> possibleMoves;
	Player* player;
	Player* enemy;
	pt2di lastMoveStart{};
	pt2di lastMoveEnd{};
};