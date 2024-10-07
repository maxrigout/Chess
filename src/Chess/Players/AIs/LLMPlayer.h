#pragma once
#include "Chess/Players/AIPlayer.h"

#include "Core/HTTP/APIClient.h"
#include "Core/LLMs/gemini/GeminiAgent.h"

#include <nlohmann/json.hpp>

class LLMPlayer : public AIPlayer
{
public:
	LLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey);
	virtual ~LLMPlayer();

	virtual void PlayThread();
	void PlayThread_alt();

	void PlayThread_old();

	// TODO: chose either prompt or behavior
	// prompt based polymorphyism
	virtual std::string GetInitialYouGoFirstPrompt() const = 0;
	virtual std::string GetInitialYouGoSecondPrompt(const Move& lastMove) const = 0;
	virtual std::string GetMovePrompt(const Move& lastMove) const = 0;
	virtual nlohmann::json ParseResponse(const std::string& llmResponse) const = 0;
	// for when cannot parse the response
	virtual std::string OnDeserializationErrorPrompt(const std::string& data) const = 0;
	// for when the data in the response is invalid
	virtual std::string OnInvalidDataReceivedPrompt(const std::string& type, const std::string& data) const = 0;
	// for when out of bounds
	virtual std::string OnInvalidChessCellPrompt(const std::string& type, const pt2di& coordinates) const = 0;
	// for when no piece is found
	virtual std::string OnNoPieceFoundPrompt(const pt2di& location) const = 0;
	// for when a piece from the other team is selected
	virtual std::string OnWrongPieceChosenPrompt(const pt2di& location) const = 0;
	// for when an illegal move is attempted
	virtual std::string OnInvalidMovePrompt(const std::string& pieceType, const pt2di& start, const pt2di& end) const = 0;
	// for when an unknown error occurred
	virtual std::string OnUnknownErrorPrompt() const = 0;

	// behavior based polymorphism
	// can throw:
	// LLMDeserializationChessException
	// LLMInvalidDataChessException
	virtual Move AskForMoveYouGoFirst() = 0;
	virtual Move AskForMoveYouGoSecond(const Move& lastMove) = 0;
	virtual Move AskForMove(const Move& lastMove) = 0;

	// for when cannot parse the response
	virtual void OnDeserializationError() = 0;

	// for when the data in the response is invalid
	virtual void OnInvalidDataReceived() = 0;

	// for when out of bounds
	virtual void OnInvalidChessCell() = 0;

	// for when no piece is found
	virtual void OnNoPieceFound() = 0;

	// for when a piece from the other team is selected
	virtual void OnWrongPieceChosen() = 0;

	// for when an illegal move is attempted
	virtual void OnInvalidMove() = 0;

private:
	GeminiAgent m_agent;
	bool m_isAgentInitialized{ false };
};