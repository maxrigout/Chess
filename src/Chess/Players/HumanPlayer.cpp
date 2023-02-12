#include "HumanPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include <iostream>

HumanPlayer::HumanPlayer(Board* board, TEAM team, int king_row)
	: Player(board, team)
{
	int pawn_row = 0;
	if (king_row == 7)
	{
		pawn_row = 6;
	}
	else
	{
		pawn_row = king_row + 1;
	}
	Piece *t1, *t2, *b1, *b2, *h1, *h2, *q, *p;
	t1 = new	Rook	(m_pBoard, team, { 0, king_row });
	h1 = new	Knight	(m_pBoard, team, { 1, king_row });
	b1 = new	Bishop	(m_pBoard, team, { 2, king_row });
	q = new		Queen	(m_pBoard, team, { 3, king_row });
	m_king = new	King	(m_pBoard, team, { 4, king_row });
	b2 = new	Bishop	(m_pBoard, team, { 5, king_row });
	h2 = new	Knight	(m_pBoard, team, { 6, king_row });
	t2 = new	Rook	(m_pBoard, team, { 7, king_row });

	m_pieces = { t1, h1, b1, q, m_king, b2, h2, t2 };

	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		p = new Pawn(m_pBoard, team, { i, pawn_row });
		m_pieces.push_back(p);
	}

	for (const auto& piece : m_pieces)
	{
		board->GetCell(piece->Pos())->PlacePiece(piece);
	}
}

HumanPlayer::~HumanPlayer()
{
	for (const auto& p : m_pieces)
	{
		delete p;
	}
	m_pieces.clear();
}

void HumanPlayer::Play(const Context& context)
{
	if (!IsPieceSelected())
	{
		SelectPiece(context.selectedCellPosition);
		return;
	}

	MoveInfo info = MoveSelectedPiece(context.selectedCellPosition);
	if (info.reason != MoveInfo::NONE)
		SelectPiece(context.selectedCellPosition);
}