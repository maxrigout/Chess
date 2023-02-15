#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include <iostream>

AIPlayer::AIPlayer(Board* board, TEAM team, int king_row)
	: Player(board, team, king_row)
{
	srand(time(0));
}

AIPlayer::~AIPlayer() = default;

void AIPlayer::Play(const PlayingContext& context)
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
	Piece* pieceToMove = move.piece;
	m_lastMoveStart = pieceToMove->Pos();
	pieceToMove->Move(move.target);
	m_lastMoveEnd = pieceToMove->Pos();
	EndTurn();
}

Move AIPlayer::GetBestMove(const std::vector<Move>& moves)
{
	int max_score = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;
	for (auto& move : moves)
	{
		char old_cell = TestMove(move);
		int score = m_king->GetMod(m_team) * EvaluateBoard();
		std::cout << "evaluating: " << move.piece->Type() << " to " << m_pBoard->GetBoardCoordinates(move.target) << " score: " << score << '\n';
		if (score > max_score)
		{
			bestMoves.clear();
			bestMoves.push_back(move);
			std::cout << "found better move\n";
			max_score = score;
		}
		else if (score == max_score)
		{
			bestMoves.push_back(move);
		}
		UndoMove(move, old_cell);
	}
	// pick a random from the best moves
	return bestMoves[rand() % bestMoves.size()];
}

int AIPlayer::EvaluateBoard() const
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
int AIPlayer::GetPiecePoints(Piece* p) const
{
	if (p == nullptr)
		return 0;
	return GetPiecePoints(p->Type());
}
int AIPlayer::GetPiecePoints(char type) const
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
int AIPlayer::GetPieceValue(Piece* p) const
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
int AIPlayer::GetPieceValue(char type) const
{
	int val = (int)type;
	int mod = val / abs(val);
	return mod * GetPiecePoints(type * mod);
}

void AIPlayer::DrawLastMove(const Renderer2D* renderer) const
{
	renderer->DrawArrow(m_lastMoveStart, m_lastMoveEnd, MAGENTA);
}