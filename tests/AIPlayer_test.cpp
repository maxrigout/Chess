#include "TestRunner.h"
#include "Chess/Players/AIPlayer.h"

#include <thread>
#include <chrono>


TEST_FUNCTION(testing_GetPiecePoints)
{
	Board board(8, 8);
	AIPlayer player(&board, TEAM::ONE);
	auto value = player.GetPiecePoints('P');
	IS_EQUAL(10, value);
}

TEST_FUNCTION(testing_EvaluateBoard)
{

	struct Move
	{
		pt2di from;
		pt2di to;
	};

	Board board(8, 8);
	HumanPlayer player(&board, TEAM::ONE);
	AIPlayer ai(&board, TEAM::TWO);
	ai.SetOpponentPlayer(&player);
	srand(0);

	// std::vector<Move> moves = { { { 4, 6 }, { 4, 4 } }, { { 4, 4 }, { 4, 3 } }, {{ 5, 6 }, { 5, 4 } } };
	std::vector<Move> moves = { { { 4, 6 }, { 4, 5 } }, { { 4, 5 }, { 4, 4 } }, { { 4, 4 }, { 4, 3 } }, {{ 5, 6 }, { 5, 4 } } };

	auto aiPlay = [&ai, &board]() {
			ai.BeginTurn();
			while (!ai.HasEndedTurn())
			{
				ai.Play({});
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			std::cout << "\nplayer\n" << board.ToString() << std::endl;
		};
	auto playerPlay = [&player, &board](const pt2di& from, const pt2di& to) {
			player.BeginTurn();
			PlayingContext ctx;
			ctx.selectedCellPosition = from;
			player.Play(ctx);
			ctx.selectedCellPosition = to;
			player.Play(ctx);
			std::cout << "\nplayer\n" << board.ToString() << std::endl;
		};

	for (const auto& move : moves)
	{
		aiPlay();
		playerPlay(move.from, move.to);
	}

	aiPlay();

	auto value = ai.EvaluateBoard2();

	IS_EQUAL(20, value);
}