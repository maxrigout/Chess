#define CHESS_CONFIG_IMPL
#include "Chess/Game.h"
#include "Core/Logger.h"

int main(int argc, char** argv)
{
	LOG_DEBUG("hello");
	LOG_DEBUG(argv[0]);
	Game game;
	game.Init(800, 600);
	game.Run();
	game.Cleanup();

	return 0;
}
