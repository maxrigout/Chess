#include "NetworkPlayer.h"

#include "Core/Logger.h"


NetworkPlayer::NetworkPlayer(Board* pBoard, TEAM team)
	: Player(pBoard, team)
{
	// TODO: need to initialize winsock here (windows)...

	// TODO: need to configure the play: should probably be its own class
	//	hosting or joining
	//	which ip to use?
	// did the other accept?

	// TODO: need to disable undo as well...
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
		m_isPlaying = true;
		if (m_playThread.joinable())
			m_playThread.join();
		m_shouldStopProcessing = false;
		m_playBegin = std::chrono::high_resolution_clock::now();
		m_playThread = std::thread(&NetworkPlayer::PlayThread, this);
	}
	else
	{
		auto now = std::chrono::high_resolution_clock::now();
		int timeEllapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_playBegin).count();
		bool timeExeeced = timeEllapsedSeconds > 10;
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

void NetworkPlayer::PlayThread()
{

	// need to send the previous move as well...
	// this should probably be in a "beginTurn" function
	if (!SendMove(Move{}))
	{
		// unable to send the message
	}

	Move move = ReceiveMove();

	Piece* pieceToMove = m_pBoard->GetPieceAtCell(move.origin);
	pieceToMove->Move(move.target);

	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;

	LOG_INFO("NetworkPlayer finished playing");

	EndTurn();
	m_isPlaying = false;
}

bool NetworkPlayer::SendMove(const Move& move) const
{
	return false;
}

Move NetworkPlayer::ReceiveMove() const
{
	NetworkMessage<Move> messageReceived = m_socket.Receive<Move>(/*TODO: should pass a context here...*/);
	if (messageReceived.header.status == MessageStatus::COMPLETE)
		return messageReceived.payload;

	return messageReceived.payload;
}