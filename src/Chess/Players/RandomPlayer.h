#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"

#include <thread>

class RandomPlayer : public Player
{
public:
	RandomPlayer(Board* pBoard, TEAM team);
	virtual ~RandomPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override;
	virtual bool AllowsUndos() const { return true; }

	void PlayThread();

private:
	std::thread m_playThread;
	bool m_isPlaying = false;

	pt2di m_lastMoveStart{ -100, -100 };
	pt2di m_lastMoveEnd{ -100, -100 };
};
