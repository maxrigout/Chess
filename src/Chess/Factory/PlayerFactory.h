#pragma once
#include "Chess/Player.h"
#include "Chess/Config/ChessConfig.h"
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
	std::unique_ptr<Player> CreatePlayer1(std::weak_ptr<Board> pBoard, PlayerType playerType);
	std::unique_ptr<Player> CreatePlayer2(std::weak_ptr<Board> pBoard, PlayerType playerType);

	static Player* CreatePlayer1(Board* pBoard, const std::string playerType = ChessConfiguration::GetPlayerOneType())
	{
		return CreatePlayer(pBoard, TEAM::ONE, playerType);
	}

	static Player* CreatePlayer2(Board* pBoard, const std::string playerType = ChessConfiguration::GetPlayerTwoType())
	{
		return CreatePlayer(pBoard, TEAM::TWO, playerType);
	}
private:
	static Player* CreatePlayer(Board* pBoard, TEAM team, const std::string playerType)
	{
		if (playerType == "human")
			return new HumanPlayer(pBoard, team);
		else if (playerType == "ai")
			return new AIPlayer(pBoard, team, ChessConfiguration::GetAIProperties().GetSearchDepth());
		return nullptr;
	}
};