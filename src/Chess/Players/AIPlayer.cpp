// #include "AIPlayer.h"
// #include "Chess/Piece.h"

// #include <iostream>

// ChessAI::ChessAI(Board* board, Player* own_pieces, Player* enemy_pieces)
// {
// 	ready = true;
// 	pBoard = board;
// 	player = own_pieces;
// 	enemy = enemy_pieces;
// 	team = TEAM::NONE;
// 	boardScore = 0;
// 	board_copy = nullptr;
// 	board_hard_copy = nullptr;
// 	if (own_pieces == nullptr)
// 	{
// 		ready = false;
// 		std::cout << "AI: own pieces is nullptr\n";
// 	}
// 	else
// 	{
// 		team = player->GetTeam();
// 	}
// 	if (enemy_pieces == nullptr)
// 	{
// 		ready = false;
// 		std::cout << "AI: enemy pieces is nullptr\n";
// 	}
// 	else
// 	{
// 		board_copy = new char* [pBoard->GetWidth()];
// 		for (int i = 0; i < pBoard->GetWidth(); i++)
// 		{
// 			board_copy[i] = new char[pBoard->GetHeight()];
// 		}
// 	}
// }
// void ChessAI::Play()
// {
// 	CopyBoard();
// 	//std::cout << PrintCopy();
// 	GetPossibleMoves();

// 	// RandomPlay
// 	//int r = rand() % possibleMoves.size();
// 	//pGame->arrow_start = possibleMoves[r].p->Pos();
// 	//possibleMoves[r].p->Move(possibleMoves[r].target);
// 	//pGame->arrow_end = possibleMoves[r].p->Pos();


// 	Move move = GetBestMove();
// 	lastMoveStart = move.p->Pos();
// 	move.PerformMove();
// 	lastMoveEnd = move.p->Pos();

// 	EndTurn();
// }
// void ChessAI::CopyBoard()
// {
// 	for (int i = 0; i < pBoard->GetWidth(); i++)
// 	{
// 		for (int j = 0; j < pBoard->GetHeight(); j++)
// 		{
// 			Piece* p = pBoard->GetCell({ i, j })->GetPiece();
// 			if (p != nullptr)
// 			{
// 				board_copy[i][j] = p->Type() * p->GetMod();
// 			}
// 			else
// 			{
// 				board_copy[i][j] = COPY_EMPTY_CELL;
// 			}
// 		}
// 	}
// }
// std::string ChessAI::PrintCopy()
// {
// 	std::string out("");
// 	for (int j = 0; j < pBoard->GetHeight(); j++)
// 	{
// 		for (int i = 0; i < pBoard->GetWidth(); i++)
// 		{
// 			out += board_copy[i][j];
// 		}
// 		out += '\n';
// 	}
// 	return out;
// }
// void ChessAI::GetPossibleMoves()
// {
// 	possibleMoves = player->GetLegalMoves();
// }
// std::vector<Move> ChessAI::GetHypotheticalMoves()
// {
// 	int team_mod = player->GetKing()->GetMod();
// 	std::vector<vector2d<int>> dir_bishop = { {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
// 	std::vector<vector2d<int>> dir_rook = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
// 	std::vector<vector2d<int>> dir_knight = { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} };
// 	std::vector<vector2d<int>> dir_king = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
// 	std::vector<vector2d<int>> dir_pawn = { {-1, -1}, {1, -1} };

// 	for (int i = 0; i < pBoard->GetWidth(); i++)
// 	{
// 		for (int j = 0; j < pBoard->GetHeight(); j++)
// 		{
// 			switch (board_copy[i][j] * team_mod)
// 			{
// 			//case 'P': CalcPawnMoves(); break;
// 			//case 'R': CalcRookMoves(); break;
// 			//case 'B': CalcBishopMoves(); break;
// 			//case 'H': CalcKnightMoves(); break;
// 			//case 'Q': CalcQueenMoves(); break;
// 			//case 'K': CalcKingMoves(); break;
// 			}
// 		}
// 	}
// 	return {};
// }
// Move ChessAI::GetBestMove()
// {
// 	Move bestMove = possibleMoves[0];
// 	int max_score = std::numeric_limits<int>::min();
// 	int min_score = std::numeric_limits<int>::max();
// 	for (auto& move : possibleMoves)
// 	{
// 		char old_cell = TestMove(move);
// 		int score = player->GetKing()->GetMod() * EvaluateBoard();
// 		std::cout << "evaluating: " << move.p->Type() << " to " << BoardCoordinates(move.target) << " score: " << score << '\n';
// 		if (score > max_score)
// 		{
// 			std::cout << "found better move\n";
// 			max_score = score;
// 			bestMove = move;
// 		}
// 		UndoMove(move, old_cell);
// 	}
// 	return bestMove;
// }
// void ChessAI::UndoMove(const Move& move, char old_cell)
// {
// 	board_copy[move.target.x][move.target.y] = old_cell;
// 	board_copy[move.p->Pos().x][move.p->Pos().y] = move.p->Type() * move.p->GetMod();
// }
// char ChessAI::TestMove(const Move& move)
// {
// 	point2d<int> s = move.p->Pos();
// 	point2d<int>e = move.target;
// 	char piece = board_copy[s.x][s.y];
// 	char old_cell = board_copy[e.x][e.y];
// 	board_copy[s.x][s.y] = COPY_EMPTY_CELL;
// 	board_copy[e.x][e.y] = piece;
// 	return old_cell;
// }
// int ChessAI::EvaluateBoard()
// {
// 	int out = 0;
// 	for (int i = 0; i < pBoard->GetWidth(); i++)
// 	{
// 		for (int j = 0; j < pBoard->GetHeight(); j++)
// 		{
// 			out += GetPieceValue(board_copy[i][j]);
// 		}
// 	}
// 	return out;
// }
// int ChessAI::GetPiecePoints(Piece* p)
// {
// 	if (p == nullptr)
// 		return 0;
// 	return GetPiecePoints(p->Type());
// }
// int ChessAI::GetPiecePoints(char type)
// {
// 	switch (type)
// 	{
// 	case 'P': return 10;
// 	case 'H': return 30;
// 	case 'B': return 30;
// 	case 'R': return 50;
// 	case 'Q': return 90;
// 	case 'K': return 900;
// 	default: return 0;
// 	}
// 	return 0;
// }
// int ChessAI::GetPieceValue(Piece* p)
// {
// 	int mod = 1;
// 	if (p != nullptr)
// 	{
// 		if (p->IsSameTeam(team))
// 		{
// 			mod = -1;
// 		}
// 	}
// 	return mod * GetPiecePoints(p);
// }
// int ChessAI::GetPieceValue(char type)
// {
// 	int val = (int)type;
// 	int mod = val / abs(val);
// 	return mod * GetPiecePoints(type * mod);
// }
// bool ChessAI::IsCheck()
// {
// 	bool found_king = false;
// 	point2d<int> king_pos;
// 	int team_mod = player->GetKing()->GetMod();
// 	int enemy_team_mod = -team_mod;
// 	std::vector<point2d<int>> enemy_pieces_pos;
// 	for (int i = 0; i < pBoard->GetWidth(); i++)
// 	{
// 		for (int j = 0; j < pBoard->GetHeight(); j++)
// 		{
// 			char p = board_copy[i][j];
// 			int mod = 1;
// 			if (p != 0)
// 			{
// 				mod = p / abs(p);
// 			}
// 			if (p * mod == 'K' && mod == team_mod)
// 			{
// 				king_pos.x = i;
// 				king_pos.y = j;
// 			}
// 			else if (mod != team_mod && p != COPY_EMPTY_CELL)
// 			{
// 				enemy_pieces_pos.push_back({ i, j });
// 			}
// 		}
// 	}

// 	auto inBounds = [&](point2d<int> c)
// 	{
// 		return (c.x >= 0 && c.x < pBoard->GetWidth() && c.y >= 0 && c.y < pBoard->GetHeight());
// 	};
// 	auto isEmptyCell = [&](point2d<int> c)
// 	{
// 		return (board_copy[c.x][c.y] == COPY_EMPTY_CELL);
// 	};

// 	std::vector<vector2d<int>> dir_bishop = { {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
// 	std::vector<vector2d<int>> dir_rook = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
// 	std::vector<vector2d<int>> dir_knight = { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} };
// 	std::vector<vector2d<int>> dir_king = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
// 	std::vector<vector2d<int>> dir_pawn = { {-1, -1}, {1, -1} };

// 	for (auto& d : dir_bishop)
// 	{
// 		bool blocked = false;
// 		for (int i = 1; i < 9; i++)
// 		{
// 			point2d<int> cell = king_pos + d * i;
// 			if (inBounds(cell))
// 			{
// 				if (!isEmptyCell(cell))
// 				{
// 					char piece = board_copy[cell.x][cell.y] * enemy_team_mod;
// 					switch (piece)
// 					{
// 					case 'B': return true;
// 					case 'Q': return true;
// 					default: blocked = true; break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				break;
// 			}
// 			if (blocked) break;
// 		}
// 	}
// 	for (auto& d : dir_rook)
// 	{
// 		bool blocked = false;
// 		for (int i = 1; i < 9; i++)
// 		{
// 			point2d<int> cell = king_pos + d * i;
// 			if (inBounds(cell))
// 			{
// 				if (!isEmptyCell(cell))
// 				{
// 					char piece = board_copy[cell.x][cell.y] * enemy_team_mod;
// 					switch (piece)
// 					{
// 					case 'R': return true;
// 					case 'Q': return true;
// 					default: blocked = true; break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				break;
// 			}
// 			if (blocked) break;
// 		}
// 	}
// 	for (auto& d : dir_knight)
// 	{
// 		point2d<int> cell = king_pos + d;
// 		if (inBounds(cell))
// 		{
// 			char piece = board_copy[cell.x][cell.y] * enemy_team_mod;
// 			switch (piece)
// 			{
// 			case 'H': return true;
// 			default: break;
// 			}
// 		}
// 	}
// 	for (auto& d : dir_king)
// 	{
// 		point2d<int> cell = king_pos + d;
// 		if (inBounds(cell))
// 		{
// 			char piece = board_copy[cell.x][cell.y] * enemy_team_mod;
// 			switch (piece)
// 			{
// 			case 'K': return true;
// 			default: break;
// 			}
// 		}
// 	}
// 	for (auto& d : dir_pawn)
// 	{
// 		point2d<int> cell = king_pos + d * enemy_team_mod;
// 		if (inBounds(cell))
// 		{
// 			char piece = board_copy[cell.x][cell.y] * enemy_team_mod;
// 			switch (piece)
// 			{
// 			case 'K': return true;
// 			default: break;
// 			}
// 		}
// 	}
// 	return false;
// }
