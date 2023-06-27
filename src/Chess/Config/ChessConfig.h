#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class ChessConfiguration
{
public:
	static const std::string& GetPlayerOneType()
	{
		// return s_player1Type;
		return ConfigReader::GetFile("config.yml")["player1"].GetStringValue();
	}
	static const std::string& GetPlayerTwoType()
	{
		// return s_player2Type;
		return ConfigReader::GetFile("config.yml")["player2"].GetStringValue();
	}
private:
	// static std::string s_player1Type;
	// static std::string s_player2Type;
};