#include "GeminiLLMPlayer.h"

#include "Core/Logger.h"

#include "Chess/Exceptions/LLMExceptions.h"

#include <nlohmann/json.hpp>

static std::string initialPrompt = "you are playing chess with me. You need to give me your moves with a start cell and end cell. return your moves in a json format like so: { \"start\": \"<startCell>\", \"end\": \"<endCell>\" }. ";

GeminiLLMPlayer::GeminiLLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey)
	: LLMPlayer(pBoard, team, apiKey), m_agent(apiKey)
{

}

std::string GeminiLLMPlayer::GetInitialYouGoFirstPrompt() const
{
	return initialPrompt + "You go first.";
}

std::string GeminiLLMPlayer::GetInitialYouGoSecondPrompt(const Move& lastMove) const
{
	return initialPrompt + "I'll go first: {\"start\": \"" + m_pBoard->GetBoardCoordinates(lastMove.origin) + "\", \"end\": \"" + m_pBoard->GetBoardCoordinates(lastMove.target) + "\"}";
}

std::string GeminiLLMPlayer::GetMovePrompt(const Move& lastMove) const
{
	return std::format("{\"start\": \"{}\" \"end\": \"{}\"}", m_pBoard->GetBoardCoordinates(lastMove.origin), m_pBoard->GetBoardCoordinates(lastMove.target));
}

nlohmann::json GeminiLLMPlayer::ParseResponse(const std::string& llmResponse)
{
	try
	{
		return nlohmann::json::parse(llmResponse);
	}
	catch (...)
	{
		throw LLMDeserializationChessException(llmResponse);
	}
}

Move GeminiLLMPlayer::AskForMoveYouGoFirst()
{

}

Move GeminiLLMPlayer::AskForMoveYouGoSecond(const Move& lastMove)
{
	
}

Move GeminiLLMPlayer::AskForMove(const Move& lastMove)
{

}

void GeminiLLMPlayer::OnInvalidMove()
{

}

void GeminiLLMPlayer::OnInvalidDataReceived()
{

}

void GeminiLLMPlayer::OnWrongPieceChosen()
{

}

void GeminiLLMPlayer::OnNoPieceFound()
{

}
