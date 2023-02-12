#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"
#include "Chess/Pieces/King.h"

class HumanPlayer : public Player
{
public:
	HumanPlayer(Board* board, TEAM t, int king_row);
	~HumanPlayer() override;
	void CopyBoard();
	std::vector<Move> GetPossibleMoves();
	bool IsCheck();
	bool IsCheckMate();
	bool IsMoveValid(const Move& move);
	void UndoMove(const Move& move, char old_cell);
	char TestMove(const Move& move);
	const std::vector<Move>& GetLegalMoves() const { return m_legalMoves; }
	void ResetLegalMoves() { moves_calculated = false; m_legalMoves.clear(); }
	bool HaveMovesBeenCalculated() { return moves_calculated; }
	
private:
	bool moves_calculated = false;
};