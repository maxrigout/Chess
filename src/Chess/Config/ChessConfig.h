#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class AIProperties
{
public:
	void Init(const ConfigReader::Property& props)
	{
		m_searchDepth = props["search-depth"].GetIntValue();
		m_type = props["type"].GetStringValue();
		m_timeoutMs = props["timeout-ms"].GetIntValue();
	}

	int GetSearchDepth() const
	{
		return m_searchDepth;
	}

	const std::string& GetType() const
	{
		return m_type;
	}

	int GetTimeoutMs() const
	{
		return m_timeoutMs;
	}

private:
	static int s_searchDepth;
	static std::string s_type;
	static int s_timeoutMs;

	int m_searchDepth;
	std::string m_type;
	int m_timeoutMs;
};

class LLMProperties
{
public:
	void Init(const ConfigReader::Property& props)
	{
		m_provider = props["provider"].GetStringValue();
	}

	const std::string& GetProvider() const
	{
		return m_provider;
	}

private:
	std::string m_provider;
};

class ChessConfiguration
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_player1Type = props["player1"].GetStringValue();
		s_player2Type = props["player2"].GetStringValue();
		s_aiProperties.Init(props["ai-properties"]);
		s_llmProperties.Init(props["llm-properties"]);
	}
	static const std::string& GetPlayerOneType()
	{
		return s_player1Type;
	}
	static const std::string& GetPlayerTwoType()
	{
		return s_player2Type;
	}
	static const AIProperties& GetAIProperties()
	{
		return s_aiProperties;
	}

	static const LLMProperties GetLLMProperties()
	{
		return s_llmProperties;
	}
private:
	ChessConfiguration() {}
	static std::string s_player1Type;
	static std::string s_player2Type;
	static AIProperties s_aiProperties;
	static LLMProperties s_llmProperties;
};

#ifdef CHESS_CONFIG_IMPL
#undef CHESS_CONFIG_IMPL
std::string ChessConfiguration::s_player1Type = "";
std::string ChessConfiguration::s_player2Type = "";
AIProperties ChessConfiguration::s_aiProperties;
LLMProperties ChessConfiguration::s_llmProperties;

int AIProperties::s_searchDepth = 1;
std::string AIProperties::s_type = "";
int AIProperties::s_timeoutMs = 0;
#endif