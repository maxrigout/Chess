#pragma once

#include "Chess/Player.h"
#include "Math/point2d.h"
#include "Core/Networking/Socket.h"

#include <thread>
#include <chrono>

class NetworkPlayer : public Player
{
public:
	NetworkPlayer(Board* pBoard, TEAM team);
	virtual ~NetworkPlayer();

	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;

private:

	void PlayThread();

	Socket m_socket;

	pt2di m_lastMoveStart{-100, -100};
	pt2di m_lastMoveEnd{-100, -100};

	std::thread m_playThread;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_playBegin;
	bool m_isPlaying = false;
	bool m_shouldStopProcessing = false;
	bool m_isGameFirstMove = true;

};
