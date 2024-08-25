#pragma once
#include "Chess/Players/AIPlayer.h"

#include "Core/HTTP/APIClient.h"
#include "Core/LLMs/gemini/GeminiAgent.h"

class LLMPlayer : public AIPlayer
{
public:
	LLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey);
	~LLMPlayer() = default;

	virtual void PlayThread();

private:
	GeminiAgent m_agent;
	std::vector<std::string> m_history;
	bool m_isAgentInitialized{ false };
};