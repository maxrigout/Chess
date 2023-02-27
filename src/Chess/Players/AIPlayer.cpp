#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Logger.h"

// TODO add to configuration
unsigned int searchDepth = 3;

AIPlayer::AIPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
{
	long seed = time(0);
	srand(seed);
	LOG_DEBUG("AIPlayer seed: " + std::to_string(seed));
}

AIPlayer::~AIPlayer()
{
	if (m_playThread.joinable())
		m_playThread.join();
}

void AIPlayer::Play(const PlayingContext& context)
{
	// create a thread is it's not playing
	if (!m_isPlaying)
	{
		if (m_playThread.joinable())
			m_playThread.join();
		m_playThread = std::thread(&AIPlayer::PlayThread, this);
		m_pBoard->ShowHourglass("AI Player thinking...");
	}
	else
	{
		m_pBoard->UpdateHourglass(context.dt);
	}
}

void AIPlayer::PlayThread()
{
	m_stackSizeAtBeginningOfTurn = m_pBoard->GetStackSize();
	m_isPlaying = true;
	CopyBoard();
	std::vector<Move> possibleMoves = GetPossibleMoves();

	// RandomPlay
	//int r = rand() % possibleMoves.size();
	//pGame->arrow_start = possibleMoves[r].p->Pos();
	//possibleMoves[r].p->Move(possibleMoves[r].target);
	//pGame->arrow_end = possibleMoves[r].p->Pos();

	std::vector<Move> bestMoves = GetBestMoves2(possibleMoves);
	Move move = bestMoves[rand() % bestMoves.size()];
	Piece* pieceToMove = m_pBoard->GetPieceAtCell(move.origin);
	pieceToMove->Move(move.target);
	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;
	LOG_INFO("AI Player finished playing");
	EndTurn();
	m_isPlaying = false;
}

std::vector<Move> AIPlayer::GetBestMoves2(const std::vector<Move>& moves)
{
	int maxScore = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;
	LOG_INFO("analyzing " + std::to_string(moves.size()) + " moves");
	int i = 0;
	for (const auto& move : moves)
	{
		Board board(*m_pBoard);
		TestMove2(&board, move);
		// we're searching searchDepth - 1 because we're already searching level 1 in this function.
		int moveScore = alphabeta(&board, searchDepth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
		// int moveScore = minimax(&board, searchDepth - 1, false);
		char message[200] = { 0 };
		Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
		snprintf(message, 200, "move %d: %c (%s -> %s) - score %d (%d)",
			i++,
			piece->Type(),
			m_pBoard->GetBoardCoordinates(move.origin).c_str(),
			m_pBoard->GetBoardCoordinates(move.target).c_str(),
			moveScore,
			maxScore);
		LOG_DEBUG(message);
		if (moveScore > maxScore)
		{
			LOG_INFO("found better move");
			maxScore = moveScore;
			bestMoves.clear();
		}
		if (moveScore >= maxScore)
			bestMoves.push_back(move);
		// UndoMove2(&board);
	}
	char message[200] = { 0 };
	snprintf(message, 200, "found %lu moves. Max score %d",
		bestMoves.size(),
		maxScore);
	LOG_DEBUG(message);
	return bestMoves;
}

int AIPlayer::minimax(Board* pBoard, int depth, bool isMaximizingPlayer)
{
	// TODO: properly handle checkmates
	if (depth <= 0 || IsCheckMate())
		return EvaluateBoard2(pBoard);

	if (isMaximizingPlayer)
	{
		int max_score = std::numeric_limits<int>::min();
		std::vector<Move> moves = GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(pBoard, move);
			int score = minimax(pBoard, depth - 1, !isMaximizingPlayer);
			if (score > max_score)
				max_score = score;
			UndoMove2(pBoard);
		}
		return max_score;
	}
	else
	{
		int min_score = std::numeric_limits<int>::max();
		m_opponentPlayer->CalculateLegalMoves();
		if (m_opponentPlayer->IsCheckMate())
			return 10000;
		std::vector<Move> moves = m_opponentPlayer->GetPossibleMoves();
		// if (MovesHaveCastle(moves))
		// 	LOG_DEBUG("opponent moves has a castle");
		// for (const auto& move : moves)
		for (int i = 0; i < moves.size(); ++i)
		{
			const auto& move = moves[i];
			TestMove2(pBoard, move);
			int score = minimax(pBoard, depth - 1, !isMaximizingPlayer);
			if (score < min_score)
				min_score = score;
			UndoMove2(pBoard);
		}
		return min_score;
	}
	return 0;
}

int AIPlayer::alphabeta(Board* pBoard, int depth, int alpha, int beta, bool isMaximizingPlayer)
{
	// TODO: properly handle checkmates
	if (depth <= 0 || IsCheckMate())
		return EvaluateBoard2(pBoard);

	int score = 0;
	if (isMaximizingPlayer)
	{
		score = std::numeric_limits<int>::min();
		DummyPlayer CopyOfThis(pBoard, m_team);
		CopyOfThis.CalculateLegalMoves();
		std::vector<Move> moves = CopyOfThis.GetPossibleMoves();
		if (moves.size() == 0)
			return -10000;
		for (const auto& move : moves)
		{
			Board board(*pBoard);
			TestMove2(&board, move);
			score = std::max(score, alphabeta(&board, depth - 1, alpha, beta, !isMaximizingPlayer));
			// UndoMove2(pBoard);
			if (score > beta)
                break; // (* β cutoff *)
            alpha = std::max(alpha, score);
		}
	}
	else
	{
		score = std::numeric_limits<int>::max();
		DummyPlayer copyOfOpponent(pBoard, m_team == TEAM::ONE? TEAM::TWO : TEAM::ONE);
		copyOfOpponent.CalculateLegalMoves();
		if (copyOfOpponent.IsCheckMate())
		{
			LOG_INFO("checkmate");
			return 10000;
		}
		std::vector<Move> moves = copyOfOpponent.GetPossibleMoves();
		// if (MovesHaveCastle(moves))
		// 	LOG_DEBUG("opponent moves has a castle");
		for (const auto& move : moves)
		{
			Board board(*pBoard);
			TestMove2(&board, move);
			score = std::min(score, alphabeta(&board, depth - 1, alpha, beta, !isMaximizingPlayer));
			// UndoMove2(pBoard);
			if (score < alpha)
                break; //(* α cutoff *)
            beta = std::min(beta, score);
		}
	}
	return score;
}

// should probably move the TestMove2 and UndoMove2 to the Board class
void AIPlayer::TestMove2(Board* pBoard, const Move& move)
{
	// move.piece->Move(move.target);
	Piece* p = pBoard->GetPieceAtCell(move.origin);
	p->Move(move.target);
}

void AIPlayer::UndoMove2(Board* pBoard)
{
	pBoard->UndoMove();
}

int AIPlayer::EvaluateBoard2(Board* pBoard) const
{
	int out = 0;
	for (int i = 0; i < pBoard->GetWidth(); i++)
	{
		for (int j = 0; j < pBoard->GetHeight(); j++)
		{
			out += GetPieceValue(pBoard->GetPieceAtCell({ i, j }));
		}
	}

	return out;
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
	int mod = (int)piece->IsSameTeam(m_team) * 2 - 1; // 1 for same team, -1 for opponent team
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
	vec2di offset = m_pBoard->GetCellDim() / 2;
	pt2di arrowStart = m_pBoard->BoardToWindowCoordinates(m_lastMoveStart) + offset;
	pt2di arrowEnd = m_pBoard->BoardToWindowCoordinates(m_lastMoveEnd) + offset;
	renderer->DrawArrow(arrowStart, arrowEnd, MAGENTA);
}