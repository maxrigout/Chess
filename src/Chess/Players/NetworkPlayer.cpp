#include "NetworkPlayer.h"

#include "Core/Logger.h"

using namespace std::chrono_literals;


NetworkPlayer::NetworkPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
{
	// TODO: need to initialize winsock here (windows)...
	// would probably need some sort of networking class

	// TODO: would need some sort of chat

	// TODO: need to configure the play
	//     should probably be its own class (in the future)
	//         maybe in the config
	//	hosting or joining
	//	which ip to use?
	//  did the other accept?
	//  handshake
	//     game version
	//     send verification token of the initial board, make sure they match
	//     host will be white
	//     joiner will be black
}

NetworkPlayer::~NetworkPlayer()
{
	if (m_playThread.joinable())
		m_playThread.join();
}

void NetworkPlayer::Play(const PlayingContext& context)
{
	if (!m_isPlaying)
	{
		m_isGameFirstMove = context.turnNumber != 0;
		m_isPlaying = true;
		// TODO: recycle thread
		if (m_playThread.joinable())
			m_playThread.join();
		m_shouldStopProcessing = false;
		m_playBegin = std::chrono::high_resolution_clock::now();
		m_playThread = std::thread(&NetworkPlayer::PlayThread, this);
		// need to send the last move and the board verification token
	}
	else
	{
		// need to wait for move from the other player
		// play the move and verify the verification token
		// what to do if we receive a bad verification token?
		// should we really add a delay?
		auto now = std::chrono::high_resolution_clock::now();
		auto timeEllapsedSeconds = now - m_playBegin;
		bool timeExeeced = timeEllapsedSeconds > 10s;
		if (timeExeeced)
			LOG_INFO("time exceeded!");
		m_shouldStopProcessing = context.shouldQuit || timeExeeced;
		m_pBoard->UpdateHourglass(context.dt);
	}
}

void NetworkPlayer::DrawLastMove(const Renderer2D* renderer) const
{
	vec2di offset = m_pBoard->GetCellDim() / 2;
	pt2di arrowStart = m_pBoard->BoardToWindowCoordinates(m_lastMoveStart) + offset;
	pt2di arrowEnd = m_pBoard->BoardToWindowCoordinates(m_lastMoveEnd) + offset;
	renderer->DrawArrow(arrowStart, arrowEnd, MAGENTA);
}

// need to use the session
void NetworkPlayer::PlayThread()
{
	if (!m_isGameFirstMove)
	{
		Move lastMove = m_pBoard->GetLastMove();
		// TODO: calculate the verification token
		// TODO: send move
	}
	// TODO: send the verification token of the board too
	// TODO: handle communication errors
	// unable to send the message

	// this is ok since we're recieving from another thread... can't quit easily tho
	// would need 
	// Move move = ReceiveMove();
	Move move{};

	Piece* pieceToMove = m_pBoard->GetPieceAtCell(move.origin);
	pieceToMove->Move(move.target);

	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;

	LOG_INFO("NetworkPlayer finished playing");

	EndTurn();
	m_isPlaying = false;
}
