#pragma once

#include "Renderer2D/Renderer2D.h"
#include "Chess/Common.h"
#include "Chess/Board.h"
#include "Chess/Piece.h"

#include <vector>
#include <memory>

struct Context
{
	pt2di selectedCellPosition;
	int processingDelay;
};

class Player
{
public:
	Player(Board* board, TEAM team);
	virtual ~Player() = default;
	virtual void Play(const Context& context) = 0;
	void CalculateMoves();
	Piece* GetPieceAtPosition(const pt2di& position);
	void DrawPieces(const Renderer2D* renderer) const;
	void DrawSelectedPieceMoves(const Renderer2D* renderer) const;
	void GuardCells();
	TEAM GetTeam() const { return m_team; }
	bool HasEndedTurn() const { return m_hasTurnEnded; }
	void BeginTurn() { m_hasTurnEnded = false; }
	King* GetKing() { return m_king; }
	void CopyBoard();
	bool IsCheck() const;
	bool IsCheckMate() const;
	bool IsMoveValid(const Move& move) const;
	std::vector<Move> GetPossibleMoves();
	bool IsPieceSelected() { return m_selectedPiece != nullptr; }
	void SelectPiece(const pt2di& position);
	MoveInfo MoveSelectedPiece(const pt2di& position);

protected:
	void UndoMove(const Move& move, char old_cell);
	char TestMove(const Move& move);
	void EndTurn() { m_hasTurnEnded = true; }
	char& GetCopiedCell(int x, int y) { return board_copy[y * m_board->GetWidth() + x]; }
	char GetCopiedCell(int x, int y) const { return board_copy[y * m_board->GetWidth() + x]; }

	std::unique_ptr<char[]> board_copy;
	Board* m_board = nullptr;
	TEAM m_team = TEAM::NONE;

	std::vector<Piece*> m_pieces;
	King* m_king = nullptr;
	Piece* m_selectedPiece = nullptr;

	std::vector<Move> m_legalMoves;
	bool m_movesCalculated = false;

private:
	bool m_hasTurnEnded = false;
};

#include "Chess/Players/HumanPlayer.h"
#include "Chess/Players/AIPlayer.h"