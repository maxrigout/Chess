#include "Chess/Game.h"

int main(int argc, char** argv)
{
	Game game;
	game.Init(800, 600);
	game.Run();
	game.Cleanup();
	return 0;
}