#include "AIPlayer.h"
#include "Chess/Piece.h"

#include <iostream>

AIPlayer::AIPlayer(Board* board, TEAM team, int king_row)
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

	for (int i = 0; i < m_pBoard->GetWidth(); ++i)
	{
		p = new Pawn(m_pBoard, team, { i, pawn_row });
		m_pieces.push_back(p);
	}

	for (const auto& piece : m_pieces)
	{
		board->GetCell(piece->Pos())->PlacePiece(piece);
	}
}

AIPlayer::~AIPlayer()
{
	for (const auto& p : m_pieces)
	{
		delete p;
	}
	m_pieces.clear();
}

void AIPlayer::Play(const Context& context)
{
	CopyBoard();
	//std::cout << PrintCopy();
	std::vector<Move> possibleMoves = GetPossibleMoves();

	// RandomPlay
	//int r = rand() % possibleMoves.size();
	//pGame->arrow_start = possibleMoves[r].p->Pos();
	//possibleMoves[r].p->Move(possibleMoves[r].target);
	//pGame->arrow_end = possibleMoves[r].p->Pos();

	Move move = GetBestMove(possibleMoves);
	lastMoveStart = move.p->Pos();
	move.PerformMove();
	lastMoveEnd = move.p->Pos();

	EndTurn();
}

// std::string AIPlayer::PrintCopy()
// {
// 	std::string out("");
// 	for (int j = 0; j < pBoard->GetHeight(); j++)
// 	{
// 		for (int i = 0; i < pBoard->GetWidth(); i++)
// 		{
// 			out += board_copy[i][j];
// 		}
// 		out += '\n';
// 	}
// 	return out;
// }

Move AIPlayer::GetBestMove(const std::vector<Move>& moves)
{
	Move bestMove = moves[0];
	int max_score = std::numeric_limits<int>::min();
	int min_score = std::numeric_limits<int>::max();
	for (auto& move : moves)
	{
		char old_cell = TestMove(move);
		int score = m_king->GetMod(m_team) * EvaluateBoard();
		std::cout << "evaluating: " << move.p->Type() << " to " << m_pBoard->GetBoardCoordinates(move.target) << " score: " << score << '\n';
		if (score > max_score)
		{
			std::cout << "found better move\n";
			max_score = score;
			bestMove = move;
		}
		UndoMove(move, old_cell);
	}
	return bestMove;
}

int AIPlayer::EvaluateBoard()
{
	int out = 0;
	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		for (int j = 0; j < m_pBoard->GetHeight(); j++)
		{
			out += GetPieceValue(GetCopiedCell(i, j));
		}
	}
	return out;
}
int AIPlayer::GetPiecePoints(Piece* p)
{
	if (p == nullptr)
		return 0;
	return GetPiecePoints(p->Type());
}
int AIPlayer::GetPiecePoints(char type)
{
	switch (type)
	{
	case 'P': return 10;
	case 'H': return 30;
	case 'B': return 30;
	case 'R': return 50;
	case 'Q': return 90;
	case 'K': return 900;
	default: return 0;
	}
	return 0;
}
int AIPlayer::GetPieceValue(Piece* p)
{
	int mod = 1;
	if (p != nullptr)
	{
		if (p->IsSameTeam(m_team))
		{
			mod = -1;
		}
	}
	return mod * GetPiecePoints(p);
}
int AIPlayer::GetPieceValue(char type)
{
	int val = (int)type;
	int mod = val / abs(val);
	return mod * GetPiecePoints(type * mod);
}

void AIPlayer::DrawLastMove(const Renderer2D* renderer) const
{
	renderer->DrawArrow(lastMoveStart, lastMoveEnd, DARK_BLUE);
}