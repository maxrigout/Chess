#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Logger.h"

unsigned int searchDepth = 3;

AIPlayer::AIPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
{
	long seed = time(0);
	srand(seed);
	LOG_DEBUG("AIPlayer seed: " + std::to_string(seed));
	if (searchDepth % 2)
		searchDepth++;
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
	}
}

void AIPlayer::PlayThread()
{
	m_stackSizeAtBeginningOfTurn = m_pBoard->GetStackSize();
	m_isPlaying = true;
	CopyBoard();
	// LOG_INFO("\n" + GetCopyAsString());
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
	LOG_INFO("AI Player finished playing");
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
			LOG_INFO("found better move");
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
	int maxScore = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;
	LOG_INFO("analyzing " + std::to_string(moves.size()) + " moves");
	int i = 0;
	for (const auto& move : moves)
	{
		TestMove2(move);
		// we're searching searchDepth - 1 because we're already searching level 1 in this function.
		// int moveScore = alphabeta(searchDepth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
		int moveScore = minimax(searchDepth - 1, false);
		char message[200] = { 0 };
		snprintf(message, 200, "move %d: %c (%s -> %s) - score %d (%d)",
			i++,
			move.piece->Type(),
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
		UndoMove2();
	}
	char message[200] = { 0 };
	snprintf(message, 200, "found %lu moves. Max score %d",
		bestMoves.size(),
		maxScore);
	LOG_DEBUG(message);
	return bestMoves;
}

int AIPlayer::minimax(int depth, bool isMaximizingPlayer)
{
	// TODO: properly handle checkmates
	if (depth <= 0 || IsCheckMate())
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
		m_opponentPlayer->CalculateLegalMoves();
		if (m_opponentPlayer->IsCheckMate())
			return 10000;
		std::vector<Move> moves = m_opponentPlayer->GetPossibleMoves();
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
	if (depth <= 0 || IsCheckMate())
		return EvaluateBoard2();

	int score = 0;
	if (isMaximizingPlayer)
	{
		score = std::numeric_limits<int>::min();
		CalculateLegalMoves();
		std::vector<Move> moves = GetPossibleMoves();
		if (moves.size() == 0)
			return -10000;
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
		m_opponentPlayer->CalculateLegalMoves();
		if (m_opponentPlayer->IsCheckMate())
		{
			LOG_INFO("checkmate");
			return 10000;
		}
		std::vector<Move> moves = m_opponentPlayer->GetPossibleMoves();
		for (const auto& move : moves)
		{
			TestMove2(move);
			score = std::min(score, alphabeta(depth - 1, alpha, beta, !isMaximizingPlayer));
			UndoMove2();
			if (score < alpha)
                break; //(* α cutoff *)
            beta = std::min(beta, score);
		}
	}
	return score;
}

// should probably move the TestMove2 and UndoMove2 to the Board class
void AIPlayer::TestMove2(const Move& move)
{
	move.piece->Move(move.target);
}

void AIPlayer::UndoMove2()
{
	m_pBoard->UndoMove();
	if (m_stackSizeAtBeginningOfTurn > m_pBoard->GetStackSize())
		LOG_ERROR("board move stack size decreased!");
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
			out += GetPieceValue(m_pBoard->GetPieceAtCell({ i, j }));
		}
	}
	// LOG_DEBUG("\n" + m_pBoard->ToString());
	// LOG_DEBUG("evaluate board returned: " + std::to_string(out) + "\n\n");

	return out;
}

// int AIPlayer::GetPiecePoints(Piece* piece) const
// {
// 	if (piece == nullptr)
// 		return 0;
// 	return GetPiecePoints(piece->Type());
// }

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
	vec2di offset = m_pBoard->GetMargin();
	renderer->DrawArrow(m_lastMoveStart + offset, m_lastMoveEnd + offset, MAGENTA);
}