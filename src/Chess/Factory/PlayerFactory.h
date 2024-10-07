#pragma once
#include "Chess/Players/Players.h"
#include "Chess/Config/ChessConfig.h"
#include "Core/Config/SecretManager.h"
#include "Chess/Exceptions/UnknownAITypeException.h"
#include "Chess/Exceptions/UnknownPlayerTypeException.h"
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
	static std::unique_ptr<Player> CreatePlayer1(Board* pBoard, const std::string& playerType = ChessConfiguration::GetPlayerOneType())
	{
		return CreatePlayer(pBoard, TEAM::ONE, playerType);
	}

	static std::unique_ptr<Player> CreatePlayer2(Board* pBoard, const std::string& playerType = ChessConfiguration::GetPlayerTwoType())
	{
		return CreatePlayer(pBoard, TEAM::TWO, playerType);
	}

	static std::unique_ptr<AIPlayer> CreateAIPlayer(Board* pBoard, TEAM team, const std::string& aiType = ChessConfiguration::GetAIProperties().GetType())
	{
		if (aiType == "min-max")
			return std::make_unique<MinMaxAIPlayer>(pBoard, team, ChessConfiguration::GetAIProperties().GetSearchDepth(), ChessConfiguration::GetAIProperties().GetTimeoutMs());
		else if (aiType == "alpha-beta")
			return std::make_unique<AlphaBetaAIPlayer>(pBoard, team, ChessConfiguration::GetAIProperties().GetSearchDepth(), ChessConfiguration::GetAIProperties().GetTimeoutMs());

		throw std::runtime_error("unknown ai type: " + aiType);
	}

private:
	static std::unique_ptr<Player> CreatePlayer(Board* pBoard, TEAM team, const std::string& playerType)
	{
		if (playerType == "human")
			return std::make_unique<HumanPlayer>(pBoard, team);
		else if (playerType == "ai")
			return CreateAIPlayer(pBoard, team, ChessConfiguration::GetAIProperties().GetType());
		else if (playerType == "llm")
			return CreateLLMPlayer(pBoard, team, ChessConfiguration::GetLLMProperties().GetProvider());
		else if (playerType == "network")
			return std::make_unique<NetworkPlayer>(pBoard, team);
		else if (playerType == "random")
			return std::make_unique<RandomPlayer>(pBoard, team);

		throw std::runtime_error("unknown player type: " + playerType);
	}

	static std::unique_ptr<Player> CreateLLMPlayer(Board* pBoard, TEAM team, const std::string& llmProvider)
	{
		if (llmProvider == "gemini")
			return std::make_unique<GeminiLLMPlayer>(pBoard, team, SecretManager::GetSecret("gemini"));

		throw std::runtime_error("unknown llm type: " + llmProvider);
	}
};