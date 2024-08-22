#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Core/Logger.h"

using namespace std::chrono_literals;

AIPlayer::AIPlayer(Board* pBoard, TEAM team, int processingTimeoutMs)
	: Player(pBoard, team), m_processingTimeoutMs{ processingTimeoutMs }
{
	long seed = time(0);
	srand(seed);
	LOG_DEBUG("AIPlayer seed: ", seed);
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
		m_isPlaying = true;
		if (m_playThread.joinable())
			m_playThread.join();
		m_shouldStopProcessing = false;
		// TODO: reuse the thread
		m_playThread = std::thread(&AIPlayer::PlayThread, this);
		m_playBegin = std::chrono::high_resolution_clock::now();
		m_pBoard->ShowHourglass("AI Player thinking...");
	}
	else
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto timeEllapsedSeconds = now - m_playBegin;
		bool timeExeeced = timeEllapsedSeconds > m_processingTimeoutMs;
		if (timeExeeced)
			LOG_WARN("time exceeded!");
		m_shouldStopProcessing = context.shouldQuit || timeExeeced;
		m_pBoard->UpdateHourglass(context.dt);
	}
}

void AIPlayer::TestMove(Piece* piece, const Move& move)
{
	piece->Move(move.target);
}

void AIPlayer::UndoMove(Board* pBoard)
{
	pBoard->UndoMove();
}

void AIPlayer::DrawLastMove(const Renderer2D* renderer) const
{
	vec2di offset = m_pBoard->GetCellDim() / 2;
	pt2di arrowStart = m_pBoard->BoardToWindowCoordinates(m_lastMoveStart) + offset;
	pt2di arrowEnd = m_pBoard->BoardToWindowCoordinates(m_lastMoveEnd) + offset;
	renderer->DrawArrow(arrowStart, arrowEnd, MAGENTA);
}
