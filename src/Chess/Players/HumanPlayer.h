#pragma once

#include "Chess/Player.h"
#include "Chess/Board.h"
#include "Chess/Common.h"
#include "Chess/Pieces/King.h"

class HumanPlayer : public Player
{
public:
	HumanPlayer(Board* board, TEAM t, int king_row);
	~HumanPlayer();
	virtual void Play(const Context& context) override;
};