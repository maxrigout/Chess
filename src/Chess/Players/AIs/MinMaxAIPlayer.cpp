#include "MinMaxAIPlayer.h"

#include "Core/Logger.h"

MinMaxAIPlayer::MinMaxAIPlayer(Board* pBoard, TEAM team, int searchDepth, int processingTimeout)
	: AIPlayer(pBoard, team, searchDepth, processingTimeout)
{
	LOG_INFO("MinMaxAIPlayer");
}

void MinMaxAIPlayer::PlayThread()
{
	m_stackSizeAtBeginningOfTurn = m_pBoard->GetStackSize();
	CopyBoard();
#if !MAKE_COPIES
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

std::vector<Move> MinMaxAIPlayer::GetBestMoves(const std::vector<Move>& moves)
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
		int moveScore = minimax(&board, m_searchDepth - 1, false);
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

int MinMaxAIPlayer::minimax(Board* pBoard, int depth, bool isMaximizingPlayer)
{
	// TODO: cleanup
	// TODO: properly handle checkmates
	// TODO: fix nullptr
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
		DummyPlayer copyOfOpponent(pBoard, GetOpposingTeam());
		copyOfOpponent.CalculateLegalMoves();
		if (copyOfOpponent.IsCheckMate())
			return 10000;
		std::vector<Move> moves = copyOfOpponent.GetPossibleMoves();
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

int MinMaxAIPlayer::EvaluateBoard(Board* pBoard) const
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

int MinMaxAIPlayer::GetPiecePoints(char type) const
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

int MinMaxAIPlayer::GetPieceValue(Piece* piece) const
{
	if (piece == nullptr)
		return 0;
	int mod = (int)piece->IsSameTeam(m_team) * 2 - 1; // 1 for same team, -1 for opponent team
	return mod * GetPiecePoints(piece->Type());
}

int MinMaxAIPlayer::GetPieceValue(char type) const
{
	int val = (int)type;
	int mod = val / abs(val);
	return mod * GetPiecePoints(type * mod);
}
