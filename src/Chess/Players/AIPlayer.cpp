#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "DebugLogger.h"

AIPlayer::AIPlayer(Board* board, TEAM team, int king_row)
	: Player(board, team, king_row)
{
	srand(time(0));
}

AIPlayer::~AIPlayer() = default;

void AIPlayer::Play(const PlayingContext& context)
{
	CopyBoard();
	std::cout << GetCopyAsString();
	std::vector<Move> possibleMoves = GetPossibleMoves();

	// RandomPlay
	//int r = rand() % possibleMoves.size();
	//pGame->arrow_start = possibleMoves[r].p->Pos();
	//possibleMoves[r].p->Move(possibleMoves[r].target);
	//pGame->arrow_end = possibleMoves[r].p->Pos();

	std::vector<Move> bestMoves = GetBestMove(possibleMoves);
	Move move = bestMoves[rand() % bestMoves.size()];
	Piece* pieceToMove = move.piece;
	m_lastMoveStart = pieceToMove->Pos();
	pieceToMove->Move(move.target);
	m_lastMoveEnd = pieceToMove->Pos();
	EndTurn();
}

std::vector<Move> AIPlayer::GetBestMove(const std::vector<Move>& moves)
{
	int max_score = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;
	for (auto& move : moves)
	{
		// char old_cell = TestMove(move);
		// int score = m_king->GetMod(m_team) * EvaluateBoard();
		TestMove2(move);
		int score = m_king->GetMod(m_team) * EvaluateBoard2();
		// std::cout << "(" << EvaluateBoard2() << ") - (" << EvaluateBoard() << ")" << std::endl;
		// std::cout << "evaluating: " << move.piece->Type() << " to " << m_pBoard->GetBoardCoordinates(move.target) << " score: " << score << '\n';
		
		// TODO: calculate the opponent's moves
		// call GetBestMove(opponent's moves)
		
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
		// UndoMove(move, old_cell);
		UndoMove2();
	}
	// pick a random from the best moves
	return bestMoves;
}

void AIPlayer::TestMove2(const Move& move)
{
	m_movesStack.push(move);
	move.piece->Move(move.target);
}
void AIPlayer::UndoMove2()
{
	Move lastMove = m_movesStack.top();
	m_movesStack.pop();
	Piece* movedPiece = lastMove.piece;
	std::cout << "undoing moving " << movedPiece->Type() << " from " << m_pBoard->GetBoardCoordinates(lastMove.target) << " to " << m_pBoard->GetBoardCoordinates(lastMove.origin) << std::endl;
	if (movedPiece->Pos() != lastMove.target)
		LOG_DEBUG("an error might have occured");

	movedPiece->Move(lastMove.origin);

	Piece* capturedPiece = lastMove.capturedPiece;
	if (capturedPiece != nullptr)
	{
		std::cout << "resetting captured piece: " << capturedPiece->Type() << " at " << m_pBoard->GetBoardCoordinates(lastMove.target) << std::endl;
		capturedPiece->ResetCaptured();
		m_pBoard->GetCell(lastMove.target)->PlacePiece(capturedPiece);
	}
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

int AIPlayer::EvaluateBoard2() const
{
	int out = 0;
	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		for (int j = 0; j < m_pBoard->GetHeight(); j++)
		{
			out += GetPieceValue(m_pBoard->GetCell({ i, j })->GetPiece());
		}
	}
	return out;
}

int AIPlayer::GetPiecePoints(Piece* piece) const
{
	if (piece == nullptr)
		return 0;
	return GetPiecePoints(piece->Type());
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
	}
	return 0;
}

int AIPlayer::GetPieceValue(Piece* piece) const
{
	if (piece == nullptr)
		return 0;
	int mod = 1;
	if (!piece->IsSameTeam(m_team))
		mod = -1;
	return mod * GetPiecePoints(piece->Type());
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