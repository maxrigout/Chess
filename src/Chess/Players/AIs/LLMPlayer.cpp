#include "LLMPlayer.h"

#include "Core/Logger.h"

#include <nlohmann/json.hpp>

static std::string initialPrompt = "you are playing chess with me. You need to give me your moves with a start cell and end cell. return your moves in a json format like so: { \"start\": \"startCell\", \"end\": \"endCell\" }. ";

static constexpr std::string_view TeamToString(TEAM team)
{
	switch (team)
	{
		case TEAM::NONE: return "None";
		case TEAM::ONE: return "One";
		case TEAM::TWO: return "Two";
	}

	return "unknown team";
}

static bool isValidCoordinate(std::string_view coord)
{
	return coord.length() == 2
		&& ('h' - coord[0]) >= 0
		&& ('8' - coord[1]) >= 0;
}

static bool isValidCoordinate(const pt2di& pt, Board* pBoard)
{
	return pt.x >= 0 && pt.x < pBoard->GetWidth() && pt.y >= 0 && pt.y < pBoard->GetHeight();
}

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

	// validate coordinates
	if (!isValidCoordinate(origin))
	{
		LOG_ERROR("wrong origin coordinate");
	}
	if (!isValidCoordinate(target))
	{
		LOG_ERROR("wrong target coordinate");
	}

	// convert to coordinates
	Move move {
		.origin = m_pBoard->ToBoardCoordinates(origin),
		.target = m_pBoard->ToBoardCoordinates(target)
	};

	if (!isValidCoordinate(move.origin, m_pBoard))
	{
		LOG_ERROR("wrong origin coordinate");
	}

	if (!isValidCoordinate(move.target, m_pBoard))
	{
		LOG_ERROR("wrong target coordinate");
	}

	Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
	if (piece == nullptr)
	{
		LOG_ERROR("no piece found at: " + origin);
	}
	if (piece->Team() != GetTeam())
	{
		LOG_ERROR("piece not from same team! (selected piece from team ", TeamToString(piece->Team()), " when player is part of team ", TeamToString(GetTeam()), ")");
	}

	// check for invalid moves
	piece->CalculateAvailableMoves();
	const auto& availableMoves = piece->GetAvailableMoves();
	auto ite = std::find_if(availableMoves.begin(), availableMoves.end(), [move](const pt2di& m)
	{
		return m.x == move.target.x && m.y == move.target.y;
	});

	if (ite == availableMoves.end())
	{
		LOG_ERROR("not a legal move for ", piece->Type(), "! (", move.origin.x, ", ", move.origin.y, ") to (", move.target.y, ", ", move.target.y, ")");
	}
	
	piece->Move(move.target);

	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;

	LOG_INFO("LLM Player finished playing");
	EndTurn();
	m_isPlaying = false;
}