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
	// create a thread is it's not playing
	if (!m_isPlaying)
	{
		if (m_playThread.joinable())
			m_playThread.join();
		m_playThread = std::thread(&AIPlayer::PlayThread, this);
	}
}

void AIPlayer::PlayThread()
{
	m_isPlaying = true;
	CopyBoard();
	std::cout << GetCopyAsString();
	std::vector<Move> possibleMoves = GetPossibleMoves();

	// RandomPlay
	//int r = rand() % possibleMoves.size();
	//pGame->arrow_start = possibleMoves[r].p->Pos();
	//possibleMoves[r].p->Move(possibleMoves[r].target);
	//pGame->arrow_end = possibleMoves[r].p->Pos();

	std::vector<Move> bestMoves = GetBestMoves2(possibleMoves);
	Move move = bestMoves[rand() % bestMoves.size()];
	Piece* pieceToMove = move.piece;
	m_lastMoveStart = pieceToMove->Pos();
	pieceToMove->Move(move.target);
	m_lastMoveEnd = pieceToMove->Pos();
	EndTurn();
	m_isPlaying = false;
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

std::vector<Move> AIPlayer::GetBestMoves2(const std::vector<Move>& moves)
{
	int max_score = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;
	for (const auto& move : moves)
	{
		TestMove2(move);
		int score = alphabeta(3, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
		// int score = minimax(3, false);
		if (score > max_score)
		{
			std::cout << "found better move\n";
			max_score = score;
			bestMoves.clear();
		}
		if (score >= max_score)
			bestMoves.push_back(move);
		UndoMove2();
	}
	return bestMoves;
}

int AIPlayer::minimax(int depth, bool isMaximizingPlayer)
{
	// TODO: properly handle checkmates
	if (depth == 0 || IsCheckMate())
		return EvaluateBoard2();

	if (isMaximizingPlayer)
	{
		int max_score = std::numeric_limits<int>::min();
		std::vector<Move> moves = GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(move);
			int score = minimax(depth - 1, !isMaximizingPlayer);
			if (score > max_score)
				max_score = score;
			UndoMove2();
		}
		return max_score;
	}
	else
	{
		int min_score = std::numeric_limits<int>::max();
		m_enemyPlayer->CalculateMoves();
		if (m_enemyPlayer->IsCheckMate())
			return 10000;
		std::vector<Move> moves = m_enemyPlayer->GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(move);
			int score = minimax(depth - 1, !isMaximizingPlayer);
			if (score < min_score)
				min_score = score;
			UndoMove2();
		}
		return min_score;
	}
	return 0;
}

int AIPlayer::alphabeta(int depth, int alpha, int beta, bool isMaximizingPlayer)
{
	// TODO: properly handle checkmates
	if (depth == 0 || IsCheckMate())
		return EvaluateBoard2();

	int score = 0;
	if (isMaximizingPlayer)
	{
		score = std::numeric_limits<int>::min();
		std::vector<Move> moves = GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(move);
			score = std::max(score, alphabeta(depth - 1, alpha, beta, !isMaximizingPlayer));
			UndoMove2();
			if (score > beta)
                break; // (* β cutoff *)
            alpha = std::max(alpha, score);
		}
	}
	else
	{
		score = std::numeric_limits<int>::max();
		m_enemyPlayer->CalculateMoves();
		if (m_enemyPlayer->IsCheckMate())
		{
			std::cout << "checkmate\n";
			return 10000;
		}
		std::vector<Move> moves = m_enemyPlayer->GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(move);
			int score = std::min(score, alphabeta(depth - 1, alpha, beta, !isMaximizingPlayer));
			UndoMove2();
			if (score < alpha)
                break; //(* α cutoff *)
            beta = std::min(beta, score);
		}
	}
	return score;
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
	// std::cout << "undoing moving " << movedPiece->Type() << " from " << m_pBoard->GetBoardCoordinates(lastMove.target) << " to " << m_pBoard->GetBoardCoordinates(lastMove.origin) << std::endl;
	if (movedPiece->Pos() != lastMove.target)
		LOG_DEBUG("an error might have occured");

	movedPiece->Move(lastMove.origin);

	// reset the first move
	if (lastMove.wasFirstMove)
		movedPiece->ReseFirstMove();

	// reset the captured piece
	Piece* capturedPiece = lastMove.capturedPiece;
	if (capturedPiece != nullptr)
	{
		// std::cout << "resetting captured piece: " << capturedPiece->Type() << " at " << m_pBoard->GetBoardCoordinates(lastMove.target) << std::endl;
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