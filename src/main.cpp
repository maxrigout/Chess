#define CONFIG_READER_IMPL
#include "Core/Config/ConfigReader.h"
#define SYS_CONFIG_IMPL
#include "Core/Config/SystemConfiguration.h"
#define CHESS_CONFIG_IMPL
#include "Chess/Config/ChessConfig.h"
#define RENDERER_CONFIG_IMPL
#include "Renderer2D/Config/RendererConfiguration.h"
#define SECRET_CONFIG_IMPL
#include "Core/Config/SecretManager.h"

#include "Chess/Game.h"
#include "Core/Logger.h"

#include "Core/HTTP/APIClient.h"
#include "Core/LLMs/gemini/GeminiAgent.h"

int main(int argc, char** argv)
{
	const char* missingPools = getenv("OBJC_DEBUG_MISSING_POOLS");

	if (missingPools == nullptr)
	{
		LOG_ERROR("OBJC_DEBUG_MISSING_POOLS not set");
	}
	else
	{
		LOG_INFO("OBJC_DEBUG_MISSING_POOLS set to: ", missingPools);
	}

	const char* missingPools2 = getenv("OBJC_DEBUG_MISSING_POOLS_2");

	if (missingPools2 == nullptr)
	{
		LOG_ERROR("OBJC_DEBUG_MISSING_POOLS_2 not set");
	}
	else
	{
		LOG_INFO("OBJC_DEBUG_MISSING_POOLS_2 set to: ", missingPools2);
	}

	const char* missingPools3 = getenv("OBJC_DEBUG_MISSING_POOLS_3");

	if (missingPools3 == nullptr)
	{
		LOG_ERROR("OBJC_DEBUG_MISSING_POOLS_3 not set");
	}
	else
	{
		LOG_INFO("OBJC_DEBUG_MISSING_POOLS_3 set to: ", missingPools3);
	}

	try
	{
		SystemConfiguration::Init(ConfigReader::ReadYaml("config.yml")["system"]);
		ChessConfiguration::Init(ConfigReader::ReadYaml("config.yml")["chess"]);
		RendererConfiguration::Init(ConfigReader::ReadYaml("config.yml")["renderer"]);
		
		SecretManager::Init(SystemConfiguration::GetSecretsFolder());

		LOG_DEBUG("hello");
		LOG_DEBUG(argv[0]);
		Game game;
		game.Init(800, 600);
		game.Run();
		game.Cleanup();
	}
	catch (const std::exception& e)
	{
		LOG_FATAL(e.what());
	}

	return 0;
}
