#include "LLMPlayer.h"

#include "Core/Logger.h"

#include <nlohmann/json.hpp>

static std::string initialPrompt = "you are playing chess with me. You need to give me your moves with a start cell and end cell. return your moves in a json format like so: { \"start\": \"startCell\", \"end\": \"endCell\" }. ";

LLMPlayer::LLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey)
	: AIPlayer(pBoard, team, 10000), m_agent(apiKey)
{
	LOG_INFO("LLMPlayer initialized!");
}

void LLMPlayer::PlayThread()
{
	std::string prompt;
	if (!m_isAgentInitialized)
	{
		if (m_pBoard->IsFirstMove())
		{
			prompt = initialPrompt + "You go first.";
		}
		else
		{
			Move lastMove = m_pBoard->GetLastMove();
			prompt = initialPrompt + "I'll go first: {\"start\": \"" + m_pBoard->GetBoardCoordinates(lastMove.origin) + "\", \"end\": \"" + m_pBoard->GetBoardCoordinates(lastMove.target) + "\"}";
		}
		m_isAgentInitialized = true;
	}
	else
	{
		Move lastMove = m_pBoard->GetLastMove();
		prompt = "{\"start\": \"" + m_pBoard->GetBoardCoordinates(lastMove.origin) + "\" \"end\": \"" + m_pBoard->GetBoardCoordinates(lastMove.target) + "\"}";
	}

	LOG_DEBUG("sending: " + prompt);
	std::string llmResponse = m_agent.Send(prompt);
	LOG_DEBUG("received: " + llmResponse);
	nlohmann::json res = nlohmann::json::parse(llmResponse);

	std::string origin = res["start"].get<std::string>();
	std::string target = res["end"].get<std::string>();

	// convert to coordinates
	Move move {
		.origin = m_pBoard->ToBoardCoordinates(origin),
		.target = m_pBoard->ToBoardCoordinates(target)
	};

	Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
	if (piece == nullptr)
	{
		LOG_ERROR("no piece found at: " + origin);
	}
	
	piece->Move(move.target);

	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;

	LOG_INFO("LLM Player finished playing");
	EndTurn();
	m_isPlaying = false;
}