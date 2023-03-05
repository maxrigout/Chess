#include "RandomPlayer.h"
#include "Core/Logger.h"

RandomPlayer::RandomPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
{
	long seed = time(0);
	srand(seed);
	LOG_DEBUG("AIPlayer seed: " + std::to_string(seed));
}

RandomPlayer::~RandomPlayer()
{
	if (m_playThread.joinable())
		m_playThread.join();
}

void RandomPlayer::Play(const PlayingContext& context)
{
	// create a thread if it's not playing
	if (!m_isPlaying)
	{
		if (m_playThread.joinable())
			m_playThread.join();
		m_playThread = std::thread(&RandomPlayer::PlayThread, this);
		m_pBoard->ShowHourglass("RandomPlayer thinking...");
	}
	else
	{
		m_pBoard->UpdateHourglass(context.dt);
	}
}

void RandomPlayer::PlayThread()
{
	CopyBoard();
	std::vector<Move> possibleMoves = GetPossibleMoves();
	int r = rand() % possibleMoves.size();
	const Move& move = possibleMoves[r];

	Piece* pieceToMove = m_pBoard->GetPieceAtCell(move.origin);
	pieceToMove->Move(move.target);
	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;
	LOG_INFO("RandomPlayer finished playing");
	EndTurn();
	m_isPlaying = false;
}

void RandomPlayer::DrawLastMove(const Renderer2D* renderer) const
{
	vec2di offset = m_pBoard->GetCellDim() / 2;
	pt2di arrowStart = m_pBoard->BoardToWindowCoordinates(m_lastMoveStart) + offset;
	pt2di arrowEnd = m_pBoard->BoardToWindowCoordinates(m_lastMoveEnd) + offset;
	renderer->DrawArrow(arrowStart, arrowEnd, MAGENTA);
}