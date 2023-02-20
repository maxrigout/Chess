#include "Chess/Game.h"
#include "Logger.h"

int main(int argc, char** argv)
{
	LOG_DEBUG("hello");
	Game game;
	game.Init(800, 600);
	game.Run();
	game.Cleanup();
	return 0;
}