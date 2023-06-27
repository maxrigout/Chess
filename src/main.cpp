#define CHESS_CONFIG_IMPL
#include "Chess/Game.h"
#include "Core/Logger.h"
#include "Core/Config/ConfigReader.h"
#include "Core/Config/SystemConfiguration.h"
#include "Chess/Config/ChessConfig.h"
#include "Renderer2D/Config/RendererConfiguration.h"

int main(int argc, char** argv)
{
	try
	{
		// ConfigReader::DumpConfig("config.yml");
		SystemConfiguration::Init();
		ChessConfiguration::Init();
		RendererConfiguration::Init();
		LOG_DEBUG("hello");
		LOG_DEBUG(ConfigReader::GetFile("config.yml")["name"].GetStringValue());
		LOG_DEBUG(argv[0]);
		Game game;
		game.Init(800, 600);
		game.Run();
		game.Cleanup();
	}
	catch (std::exception e)
	{
		LOG_FATAL(e.what());
	}

	return 0;
}
