#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class AIProperties
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_searchDepth = props["search-depth"].GetIntValue();
		s_type = props["type"].GetStringValue();
	}

	static const int GetSearchDepth()
	{
		return s_searchDepth;
	} 

	static const std::string& GetType()
	{
		return s_type;
	}

private:
	static int s_searchDepth;
	static std::string s_type;
};

class ChessConfiguration
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_player1Type = props["player1"].GetStringValue();
		s_player2Type = props["player2"].GetStringValue();
		s_aiProperties.Init(props["ai-properties"]);
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
private:
	ChessConfiguration() {}
	static std::string s_player1Type;
	static std::string s_player2Type;
	static AIProperties s_aiProperties;
};

#ifdef CHESS_CONFIG_IMPL
#undef CHESS_CONFIG_IMPL
std::string ChessConfiguration::s_player1Type;
std::string ChessConfiguration::s_player2Type;
AIProperties ChessConfiguration::s_aiProperties;

int AIProperties::s_searchDepth;
std::string AIProperties::s_type;
#endif