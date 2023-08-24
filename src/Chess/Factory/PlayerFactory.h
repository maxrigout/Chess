#pragma once
#include "Chess/Players/Players.h"
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

	static AIPlayer* CreateAIPlayer(Board* pBoard, TEAM team, const std::string aiType = ChessConfiguration::GetAIProperties().GetType())
	{
		if (aiType == "min-max")
			return new MinMaxAIPlayer(pBoard, team, ChessConfiguration::GetAIProperties().GetSearchDepth(), ChessConfiguration::GetAIProperties().GetTimeoutMs());
		else if (aiType == "alpha-beta")
			return new AlphaBetaAIPlayer(pBoard, team, ChessConfiguration::GetAIProperties().GetSearchDepth(), ChessConfiguration::GetAIProperties().GetTimeoutMs());
		return nullptr;
	}

private:
	static Player* CreatePlayer(Board* pBoard, TEAM team, const std::string playerType)
	{
		if (playerType == "human")
			return new HumanPlayer(pBoard, team);
		else if (playerType == "ai")
			return CreateAIPlayer(pBoard, team, ChessConfiguration::GetAIProperties().GetType());
		return nullptr;
	}
};