#include "GeminiLLMPlayer.h"

#include "Core/Logger.h"

#include "Chess/Exceptions/LLMExceptions.h"

#include <nlohmann/json.hpp>
#include <format>

static std::string initialPrompt = "you are playing chess with me. You need to give me your moves with a start cell and end cell. return your moves in a json format like so: { \"start\": \"<startCell>\", \"end\": \"<endCell>\" }. ";

static std::string toJsonField(const std::string& cellType)
{
	return (cellType == "origin") ? "start" : "end";
}

static std::string teamToString(TEAM team)
{
	switch (team)
	{
		case TEAM::ONE: return "White";
		case TEAM::TWO: return "Black";
	}
}

std::string pointToBoardCoodinates(const pt2di& pt)
{
	std::stringstream boardCoordinates;
	boardCoordinates << (char)('a' + pt.x) << (char)('1' + pt.y);
	return boardCoordinates.str();
}

GeminiLLMPlayer::GeminiLLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey)
	: LLMPlayer(pBoard, team, apiKey), m_agent(apiKey)
{

}

GeminiLLMPlayer::~GeminiLLMPlayer()
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
	return std::format("{{\"start\": \"{}\" \"end\": \"{}\"}}", m_pBoard->GetBoardCoordinates(lastMove.origin), m_pBoard->GetBoardCoordinates(lastMove.target));
}

nlohmann::json GeminiLLMPlayer::ParseResponse(const std::string& llmResponse) const
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

std::string GeminiLLMPlayer::OnDeserializationErrorPrompt(const std::string& data) const
{
	return "I'm not understanding. Can you return your moves exclusively in a JSON format? For instance: { \"start\": \"d2\", \"end\": \"d4\"}.";
}

std::string GeminiLLMPlayer::OnInvalidDataReceivedPrompt(const std::string& type, const std::string& data) const
{
	std::string jsonField = toJsonField(type);
	return std::format("I don't understand what you mean for the {} (you gave: {}). Can you provide the column letter (a-h) followed by the row digit (1-8)", jsonField, data);
}

std::string GeminiLLMPlayer::OnInvalidChessCellPrompt(const std::string& type, const pt2di& coordinates) const
{
	std::string jsonField = toJsonField(type);
	return std::format("Can you provide coordinates that are on the board? (column letter (a-h) followed by the row digit (1-8))", jsonField);
}

std::string GeminiLLMPlayer::OnNoPieceFoundPrompt(const pt2di& location) const
{
	std::string boardCoordinates = pointToBoardCoodinates(location);
	return std::format("I couldn't find a piece at the location you gave: {}. Here's the board:\n{}\nYour pieces are in uppercase", boardCoordinates, GetBoardAsString());
}

std::string GeminiLLMPlayer::OnWrongPieceChosenPrompt(const pt2di& location) const
{
	std::string boardCoordinates = pointToBoardCoodinates(location);
	return std::format("The piece you selected at {} belongs to the other player. Please select another piece. (Here's the board)\n{}", boardCoordinates, GetBoardAsString());
}

std::string GeminiLLMPlayer::OnInvalidMovePrompt(const std::string& pieceType, const pt2di& start, const pt2di& end) const
{
	std::string origin = pointToBoardCoodinates(start);
	std::string target = pointToBoardCoodinates(end);
	return std::format("The piece {} cannot move from {} to {}. Please try another move.", pieceType, origin, target);
}

std::string GeminiLLMPlayer::OnUnknownErrorPrompt() const
{
	return "I don't understand what you're saying. Please provide the location (start) where the piece you'd like to move is located, and the location where you'd like that piece to move. Provide this info in a JSON format like so: { \"start\": \"<start>\", \"end\": \"<end>\"}";
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

void GeminiLLMPlayer::OnDeserializationError() {}
void GeminiLLMPlayer::OnInvalidDataReceived() {}
void GeminiLLMPlayer::OnInvalidChessCell() {}
void GeminiLLMPlayer::OnNoPieceFound() {}
void GeminiLLMPlayer::OnWrongPieceChosen() {}
void GeminiLLMPlayer::OnInvalidMove() {}
