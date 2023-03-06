#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Core/Logger.h"

#define MAKE_COPIES

// TODO add to configuration
unsigned int searchDepth = 4;

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
	// create a thread if it's not playing
	if (!m_isPlaying)
	{
		if (m_playThread.joinable())
			m_playThread.join();
		m_shouldStopProcessing = false;
		m_playBegin = std::chrono::high_resolution_clock::now();
		m_playThread = std::thread(&AIPlayer::PlayThread, this);
		m_pBoard->ShowHourglass("AI Player thinking...");
	}
	else
	{
		auto now = std::chrono::high_resolution_clock::now();
		int timeEllapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_playBegin).count();
		bool timeExeeced = timeEllapsedSeconds > context.maxProcessingDelay;
		if (timeExeeced)
			LOG_INFO("time exceeded!");
		m_shouldStopProcessing = context.shouldQuit || timeExeeced;
		m_pBoard->UpdateHourglass(context.dt);
	}
}

void AIPlayer::PlayThread()
{
	m_stackSizeAtBeginningOfTurn = m_pBoard->GetStackSize();
	m_isPlaying = true;
	CopyBoard();
#if !defined MAKE_COPIES
	m_boardCopy = std::make_unique<Board>(*m_pBoard);
	m_copyOfThis = std::make_unique<MaximizingPlayer>(m_boardCopy.get(), m_team);
	m_copyOfOpponent = std::make_unique<MinimizingPlayer>(m_boardCopy.get(), GetOpposingTeam());
#endif
	std::vector<Move> possibleMoves = GetPossibleMoves();
	std::vector<Move> bestMoves = GetBestMoves(possibleMoves);

	Move move = bestMoves[rand() % bestMoves.size()];
	Piece* pieceToMove = m_pBoard->GetPieceAtCell(move.origin);
	pieceToMove->Move(move.target);
	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;
	LOG_INFO("AI Player finished playing");
	EndTurn();
	m_isPlaying = false;
}

std::vector<Move> AIPlayer::GetBestMoves(const std::vector<Move>& moves)
{
	// TODO: cleanup
	int maxScore = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves = {};
	LOG_INFO("analyzing " + std::to_string(moves.size()) + " moves");
	int i = 0;
	Board board(*m_pBoard);
	for (const auto& move : moves)
	{
		char message[200] = { 0 };
		snprintf(message, 200, "move %d: (%s -> %s)",
			i,
			m_pBoard->GetBoardCoordinates(move.origin).c_str(),
			m_pBoard->GetBoardCoordinates(move.target).c_str());
		LOG_DEBUG(message);
		Piece* pieceToMove = board.GetPieceAtCell(move.origin);
		if (pieceToMove == nullptr)
		{
			LOG_DEBUG("\n" + board.ToString() + "\n" + board.GetBoardCoordinates(move.origin) + " -> " + board.GetBoardCoordinates(move.target));
			LOG_DEBUG("piece is nullptr");
			continue;
		}
		TestMove(pieceToMove, move);
		// we're searching searchDepth - 1 because we're already searching level 1 in this function.
		int moveScore = alphabeta(&board, searchDepth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
		// int moveScore = minimax(&board, searchDepth - 1, false);
		snprintf(message, 200, "move %d: %c (%s -> %s) - score %d (%d)",
			i,
			pieceToMove->Type(),
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
		UndoMove(&board);
		++i;
		// wrap it up if we need to quit
		if (m_shouldStopProcessing)
			break;
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
	// TODO: cleanup
	// TODO: properly handle checkmates
	if (depth <= 0 || IsCheckMate())
		return EvaluateBoard(pBoard);

	if (isMaximizingPlayer)
	{
		int max_score = std::numeric_limits<int>::min();
		std::vector<Move> moves = GetPossibleMoves();
		for (const auto& move : moves)
		{
			Piece* pieceToMove = pBoard->GetPieceAtCell(move.origin);
			TestMove(pieceToMove, move);
			int score = minimax(pBoard, depth - 1, !isMaximizingPlayer);
			if (score > max_score)
				max_score = score;
			UndoMove(pBoard);
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
		// for (const auto& move : moves)
		for (int i = 0; i < moves.size(); ++i)
		{
			const auto& move = moves[i];
			Piece* pieceToMove = pBoard->GetPieceAtCell(move.origin);
			TestMove(pieceToMove, move);
			int score = minimax(pBoard, depth - 1, !isMaximizingPlayer);
			if (score < min_score)
				min_score = score;
			UndoMove(pBoard);
		}
		return min_score;
	}
	return 0;
}

int AIPlayer::alphabeta(Board* pBoard, int depth, int alpha, int beta, bool isMaximizingPlayer)
{
	// TODO: cleanup
	// TODO: properly handle checkmates
	if (depth <= 0 || IsCheckMate() || m_shouldStopProcessing)
		return EvaluateBoard(pBoard);

	int score = 0;
	if (isMaximizingPlayer)
	{
		score = std::numeric_limits<int>::min();
		DummyPlayer CopyOfThis(pBoard, m_team);
		CopyOfThis.CalculateLegalMoves();
		std::vector<Move> moves = CopyOfThis.GetPossibleMoves();
		if (moves.size() == 0)
			return score;
		for (const auto& move : moves)
		{
#if defined MAKE_COPIES
			 Board board(*pBoard);
			 Piece* pieceToMove = board.GetPieceAtCell(move.origin);
			 TestMove(pieceToMove, move);
			 score = std::max(score, alphabeta(&board, depth - 1, alpha, beta, !isMaximizingPlayer));
#else
			Piece* pieceToMove = pBoard->GetPieceAtCell(move.origin);
			if (pieceToMove == nullptr)
			{
				LOG_DEBUG("\n" + pBoard->ToString() + "\n" + pBoard->GetBoardCoordinates(move.origin) + " -> " + pBoard->GetBoardCoordinates(move.target));
				LOG_DEBUG("piece is nullptr");
				continue;
			}
			TestMove(pieceToMove, move);
			score = std::max(score, alphabeta(pBoard, depth - 1, alpha, beta, !isMaximizingPlayer));
			UndoMove(pBoard);
#endif
			if (score > beta)
                break; // (* β cutoff *)
            alpha = std::max(alpha, score);
			// wrap it up if we need to quit
			if (m_shouldStopProcessing)
				break;
		}
	}
	else
	{
		score = std::numeric_limits<int>::max();
		DummyPlayer copyOfOpponent(pBoard, GetOpposingTeam());
		copyOfOpponent.CalculateLegalMoves();
		if (copyOfOpponent.IsCheckMate())
		{
			LOG_INFO("checkmate");
			return score;
		}
		std::vector<Move> moves = copyOfOpponent.GetPossibleMoves();
		for (const auto& move : moves)
		{
#if defined MAKE_COPIES
			 Board board(*pBoard);
			 Piece* pieceToMove = board.GetPieceAtCell(move.origin);
			 TestMove(pieceToMove, move);
			 score = std::min(score, alphabeta(&board, depth - 1, alpha, beta, !isMaximizingPlayer));
#else
			Piece* pieceToMove = pBoard->GetPieceAtCell(move.origin);
			if (pieceToMove == nullptr)
			{
				LOG_DEBUG("\n" + pBoard->ToString() + "\n" + pBoard->GetBoardCoordinates(move.origin) + " -> " + pBoard->GetBoardCoordinates(move.target));
				LOG_DEBUG("piece is nullptr");
				continue;
			}
			TestMove(pieceToMove, move);
			score = std::min(score, alphabeta(pBoard, depth - 1, alpha, beta, !isMaximizingPlayer));
			UndoMove(pBoard);
#endif
			if (score < alpha)
                break; //(* α cutoff *)
            beta = std::min(beta, score);
			// wrap it up if we need to quit
			if (m_shouldStopProcessing)
				break;
		}
	}
	return score;
}

void AIPlayer::TestMove(Piece* piece, const Move& move)
{
	piece->Move(move.target);
}

void AIPlayer::UndoMove(Board* pBoard)
{
	pBoard->UndoMove();
}

int AIPlayer::EvaluateBoard(Board* pBoard) const
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
