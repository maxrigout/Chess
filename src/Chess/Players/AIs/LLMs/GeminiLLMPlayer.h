#pragma once

#include "../LLMPlayer.h"

class GeminiLLMPlayer : public LLMPlayer
{
public:
	GeminiLLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey);
	virtual ~GeminiLLMPlayer() = default;

	virtual std::string GetInitialYouGoFirstPrompt() const;
	virtual std::string GetInitialYouGoSecondPrompt(const Move& lastMove) const;
	virtual std::string GetMovePrompt(const Move& lastMove) const;
	virtual nlohmann::json ParseResponse(const std::string& llmResponse);

	virtual Move AskForMoveYouGoFirst() = 0;
	virtual Move AskForMoveYouGoSecond(const Move& lastMove) = 0;
	virtual Move AskForMove(const Move& lastMove) = 0;
	virtual void OnInvalidMove();
	virtual void OnInvalidDataReceived();
	virtual void OnWrongPieceChosen();
	virtual void OnNoPieceFound();

private:
	GeminiAgent m_agent;
	bool m_isAgentInitialized{ false };
};