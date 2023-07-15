#define CONFIG_READER_IMPL
#include "Core/Config/ConfigReader.h"
#define SYS_CONFIG_IMPL
#include "Core/Config/SystemConfiguration.h"
#define CHESS_CONFIG_IMPL
#include "Chess/Config/ChessConfig.h"
#define RENDERER_CONFIG_IMPL
#include "Renderer2D/Config/RendererConfiguration.h"

#include "Chess/Game.h"
#include "Core/Logger.h"

int main(int argc, char** argv)
{
	try
	{
		ConfigReader::DumpConfig("config.yml");
		SystemConfiguration::Init(ConfigReader::GetFile("config.yml")["system"]);
		ChessConfiguration::Init(ConfigReader::GetFile("config.yml")["chess"]);
		RendererConfiguration::Init(ConfigReader::GetFile("config.yml")["renderer"]);
		LOG_DEBUG("hello");
		LOG_DEBUG(argv[0]);
		Game game;
		game.Init(800, 600);
		game.Run();
		game.Cleanup();
	}
	catch (std::exception& e)
	{
		LOG_FATAL(e.what());
	}

	return 0;
}
