#include "Player.h"

#include <iostream>

Player::Player(Board* board, TEAM team)
	: m_board{ board }, m_team{ team }
{
	board_copy = std::make_unique<char[]>(m_board->GetWidth() * m_board->GetHeight());
}

void Player::CalculateMoves()
{
	CopyBoard();
	if (GetKing()->Check())
	{
		std::vector<Move> possibleMoves = GetPossibleMoves();
		for (const auto& move : possibleMoves)
		{
			char old_cell = TestMove(move);
			// if the move take the king out of check
			if (!IsCheck())
			{
				m_legalMoves.push_back(move);
			}
			// undo the move
			UndoMove(move, old_cell);
		}
	}
	else
	{
		m_legalMoves = GetPossibleMoves();
	}
	m_movesCalculated = true;
}

void Player::DrawPieces(const Renderer2D* renderer) const
{
	for (const auto& piece : m_pieces)
	{
		piece->DrawYourself(renderer);
	}
}

void Player::GuardCells()
{
	for (const auto& p : m_pieces) 
	{
		if (!p->IsCaptured())
			p->GuardCells();
	}
}

Piece* Player::GetPieceAtPosition(const pt2di& position)
{
	Cell* cell = m_board->GetCell(position);
	if (cell == nullptr)
		return nullptr;
	Piece* piece = cell->GetPiece();
	if (piece == nullptr)
		return nullptr;
	if (!piece->IsSameTeam(m_team))
		return nullptr;
	return piece;
}

void Player::UndoMove(const Move& move, char old_cell)
{
	GetCopiedCell(move.target.x, move.target.y) = old_cell;
	GetCopiedCell(move.p->Pos().x, move.p->Pos().y) = move.p->Type() * move.p->GetMod();
}

char Player::TestMove(const Move& move)
{
	pt2di s = move.p->Pos();
	pt2di e = move.target;
	char piece = GetCopiedCell(s.x, s.y);
	char old_cell = GetCopiedCell(e.x, e.y);
	GetCopiedCell(s.x, s.y) = COPY_EMPTY_CELL;
	GetCopiedCell(e.x, e.y) = piece;
	return old_cell;
}

bool Player::IsCheck() const
{
	pt2di king_pos;
	int team_mod = m_king->GetMod();
	int enemy_team_mod = -team_mod;
	std::vector<pt2di> enemy_pieces_pos;
	for (int i = 0; i < m_board->GetWidth(); i++)
	{
		for (int j = 0; j < m_board->GetHeight(); j++)
		{
			char p = GetCopiedCell(i, j);
			int mod = 1;
			if (p != 0)
			{
				mod = p / abs(p);
			}
			if (p * mod == 'K' && mod == team_mod)
			{
				king_pos.x = i;
				king_pos.y = j;
			}
			else if (mod != team_mod && p != COPY_EMPTY_CELL)
			{
				enemy_pieces_pos.push_back({ i, j });
			}
		}
	}

	auto inBounds = [&](pt2di c)
	{
		return (c.x >= 0 && c.x < m_board->GetWidth() && c.y >= 0 && c.y < m_board->GetHeight());
	};
	auto isEmptyCell = [&](pt2di c)
	{
		return (GetCopiedCell(c.x, c.y) == COPY_EMPTY_CELL);
	};

	std::vector<vector2d<int>> dir_bishop = { {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
	std::vector<vector2d<int>> dir_rook = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
	std::vector<vector2d<int>> dir_knight = { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} };
	std::vector<vector2d<int>> dir_king = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
	std::vector<vector2d<int>> dir_pawn = { {-1, -1}, {1, -1} };

	for (auto& d : dir_bishop)
	{
		bool blocked = false;
		for (int i = 1; i < 9; i++)
		{
			pt2di cell = king_pos + d * i;
			if (inBounds(cell))
			{
				if (!isEmptyCell(cell))
				{
					char piece = GetCopiedCell(cell.x, cell.y) * enemy_team_mod;
					switch (piece)
					{
					case 'B': return true;
					case 'Q': return true;
					default: blocked = true; break;
					}
				}
			}
			else
			{
				break;
			}
			if (blocked) break;
		}
	}
	for (auto& d : dir_rook)
	{
		bool blocked = false;
		for (int i = 1; i < 9; i++)
		{
			pt2di cell = king_pos + d * i;
			if (inBounds(cell))
			{
				if (!isEmptyCell(cell))
				{
					char piece = GetCopiedCell(cell.x, cell.y) * enemy_team_mod;
					switch (piece)
					{
					case 'R': return true;
					case 'Q': return true;
					default: blocked = true; break;
					}
				}
			}
			else
			{
				break;
			}
			if (blocked) break;
		}
	}
	for (auto& d : dir_knight)
	{
		pt2di cell = king_pos + d;
		if (inBounds(cell))
		{
			char piece = GetCopiedCell(cell.x, cell.y) * enemy_team_mod;
			switch (piece)
			{
			case 'H': return true;
			default: break;
			}
		}
	}
	for (auto& d : dir_king)
	{
		pt2di cell = king_pos + d;
		if (inBounds(cell))
		{
			char piece = GetCopiedCell(cell.x, cell.y) * enemy_team_mod;
			switch (piece)
			{
			case 'K': return true;
			default: break;
			}
		}
	}
	for (auto& d : dir_pawn)
	{
		pt2di cell = king_pos + d * enemy_team_mod;
		if (inBounds(cell))
		{
			char piece = GetCopiedCell(cell.x, cell.y) * enemy_team_mod;
			switch (piece)
			{
			case 'K': return true;
			default: break;
			}
		}
	}
	return false;
}

bool Player::IsCheckMate() const
{
	return m_legalMoves.empty() && IsCheck();
}

bool Player::IsMoveValid(const Move& move) const
{
	for (const auto& m : m_legalMoves)
	{
		if (move.p == m.p && move.target == m.target)
		{
			return true;
		}
	}
	return false;
}

void Player::CopyBoard()
{
	for (int i = 0; i < m_board->GetWidth(); ++i)
	{
		for (int j = 0; j < m_board->GetHeight(); ++j)
		{
			Piece* p = m_board->GetCell({ i, j })->GetPiece();
			if (p != nullptr)
			{
				if (!p->IsCaptured())
				{
					GetCopiedCell(i, j) = p->Type() * p->GetMod();
				}
			}
			else
			{
				GetCopiedCell(i, j) = COPY_EMPTY_CELL;
			}
		}
	}
}

std::vector<Move> Player::GetPossibleMoves()
{
	// the algorithm is as follows:
	// for each piece,
	// verify that each allowed move for that piece, does not cause a "check"
	std::vector<Move> possibleMoves;
	std::cout << "calculating moves\n";
	for (const auto& piece : m_pieces)
	{
		if (!piece->IsCaptured())
		{
			std::vector<pt2di> availableMoves = piece->GetLegalMoves();
			for (const auto& move : availableMoves)
			{
				possibleMoves.push_back({ piece, move });
			}
		}
	}
	return possibleMoves;
}

// std::vector<Move> Player::GetPossibleMoves()
// {
// 	// the algorithm is as follows:
// 	// for each piece,
// 	// verify that each allowed move for that piece, does not cause a "check"
// 	std::vector<Move> possibleMoves;
// 	std::cout << "calculating moves\n";
// 	for (const auto& piece : m_pieces)
// 	{
// 		if (!piece->IsCaptured())
// 		{
// 			piece->ResetLegalMoves();
// 			piece->CalculateMoves();
// 			for (auto& move : piece->GetAvailableMoves())
// 			{
// 				char old_cell = TestMove({ piece, move });
// 				if (!IsCheck())
// 				{
// 					piece->AddLegalMove(move);
// 					possibleMoves.push_back({ piece, move });
// 				}
// 				GetCopiedCell(move.x, move.y) = old_cell;
// 				GetCopiedCell(piece->Pos().x, piece->Pos().y) = piece->Type() * piece->GetMod();

// 			}
// 		}
// 	}
// 	return possibleMoves;
// }