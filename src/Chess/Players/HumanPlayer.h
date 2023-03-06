#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"
#include "Chess/Pieces/King.h"

class HumanPlayer : public Player
{
public:
	HumanPlayer(Board* pBoard, TEAM team);
	virtual ~HumanPlayer();
	virtual void Play(const PlayingContext& context) override;
	virtual void DrawLastMove(const Renderer2D* renderer) const override {}
};
