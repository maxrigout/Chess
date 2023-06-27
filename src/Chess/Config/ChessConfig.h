#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class ChessConfiguration
{
public:
	static void Init()
	{
		s_player1Type = ConfigReader::GetFile("config.yml")["player1"].GetStringValue();
		s_player2Type = ConfigReader::GetFile("config.yml")["player2"].GetStringValue();
	}
	static const std::string& GetPlayerOneType()
	{
		return s_player1Type;
	}
	static const std::string& GetPlayerTwoType()
	{
		return s_player2Type;
	}
private:
	static std::string s_player1Type;
	static std::string s_player2Type;
};

#ifdef CHESS_CONFIG_IMPL
#undef CHESS_CONFIG_IMPL
std::string ChessConfiguration::s_player1Type;
std::string ChessConfiguration::s_player2Type;
#endif