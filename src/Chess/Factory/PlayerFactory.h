#pragma once
#include "Player.h"

#include <memory>

enum class PlayerType
{
	Human,
	Random,
	MinMax,
	AlphaBeta,
	Network
};

class PlayerFactory
{
public:
	std::unique_ptr<Player> CreatePlayer(std::weak_ptr<Board> pBoard, TEAM playerTeam, PlayerType playerType);
private:

};