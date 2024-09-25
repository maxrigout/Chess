#pragma once
#include "Chess/Players/AIPlayer.h"

#include "Core/HTTP/APIClient.h"
#include "Core/LLMs/gemini/GeminiAgent.h"

class LLMPlayer : public AIPlayer
{
public:
	LLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey);
	virtual ~LLMPlayer() = default;

	virtual void PlayThread();
	virtual void PlayThread_alt();

	virtual void PlayThread_old();

	virtual std::string GetInitialYouGoFirstPrompt() const = 0;
	virtual std::string GetInitialYouGoSecondPrompt(const Move& lastMove) const = 0;
	virtual std::string GetMovePrompt(const Move& lastMove) const = 0;
	virtual nlohmann::json ParseResponse(const std::string& llmResponse) = 0;

	virtual Move AskForMoveYouGoFirst() = 0;
	virtual Move AskForMoveYouGoSecond(const Move& lastMove) = 0;
	virtual Move AskForMove(const Move& lastMove) = 0;
	virtual void OnInvalidMove() = 0;
	virtual void OnInvalidDataReceived() = 0;
	virtual void OnWrongPieceChosen() = 0;
	virtual void OnNoPieceFound() = 0;

private:
	GeminiAgent m_agent;
	bool m_isAgentInitialized{ false };
};