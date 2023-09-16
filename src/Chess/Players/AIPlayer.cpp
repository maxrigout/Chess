#include "AIPlayer.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Core/Logger.h"

AIPlayer::AIPlayer(Board* pBoard, TEAM team, int searchDepth, int processingTimeout)
	: Player(pBoard, team), m_searchDepth{ searchDepth }, m_processingTimeout{ processingTimeout }
{
	LOG_INFO("AI Player initialized, searchDepth = " + std::to_string(m_searchDepth));
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
		m_isPlaying = true;
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
		bool timeExeeced = timeEllapsedSeconds > m_processingTimeout;
		if (timeExeeced)
			LOG_INFO("time exceeded!");
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
