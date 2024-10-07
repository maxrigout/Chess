#include "Player.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Core/Logger.h"
#include <sstream>

extern char to_lowercase(char);
extern char to_uppercase(char);

Player::Player(Board* board, TEAM team)
	: m_pBoard{ board }, m_team{ team }
{
	m_pBoardCopy = std::make_unique<char[]>(m_pBoard->GetWidth() * m_pBoard->GetHeight());
	m_pieces = m_pBoard->GetPlayerPieces(m_team);
	for (const auto& piece : m_pieces)
	{
		if (piece->Type() == 'K')
		{
			m_king = (King*)piece;
			break;
		}
	}
}

Player::~Player()
{
	// the player does not own the pieces anymore...
}

std::string Player::GetCopyAsString() const
{
	std::string out("+");
	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		out += '-';
	}
	out += "+\n";
	for (int j = 0; j < m_pBoard->GetHeight(); j++)
	{
		out += "|";
		for (int i = 0; i < m_pBoard->GetWidth(); i++)
		{
			out += GetCopiedCell(i, j);
		}
		out += "|\n";
	}
	out += '+';
	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		out += '-';
	}
	out += "+\n";
	return out;
}

void Player::UpdatePieces(float dt)
{
	for (auto& piece : m_pieces)
	{
		piece->UpdateMovement(dt);
	}
}

void Player::CalculateLegalMoves()
{
	for (auto& piece : m_pieces)
	{
		piece->CalculateAvailableMoves();
	}
	m_legalMoves.clear();
	CopyBoard();
	if (GetKing()->Check())
	{
		LOG_INFO("Check!\n");
		// std::cout << m_pBoard->ToString();
		std::vector<Move> possibleMoves = GetPossibleMoves();
		for (auto& piece : m_pieces)
		{
			piece->ResetAvailableMoves();
		}
		for (const auto& move : possibleMoves)
		{
			char old_cell = TestMove(move);
			// if the move take the king out of check
			if (!IsHypotheticalCheck())
			{
				m_legalMoves.push_back(move);
				m_pBoard->GetPieceAtCell(move.origin)->AddAvailableMove(move.target);
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

void Player::DrawSelectedPieceMoves(const Renderer2D* renderer) const
{
	if (m_selectedPiece != nullptr)
	{
		m_selectedPiece->DrawMoves(renderer);
	}
}

void Player::AttackCells()
{
	for (const auto& p : m_pieces) 
	{
		if (!p->IsCaptured())
			p->AttackCells();
		p->ResetAvailableMoves();
	}
}

// Piece* Player::GetPieceAtPosition(const pt2di& position)
// {
// 	Cell* cell = m_pBoard->GetCell(position);
// 	if (cell == nullptr)
// 		return nullptr;
// 	Piece* piece = cell->GetPiece();
// 	if (piece == nullptr)
// 		return nullptr;
// 	if (!piece->IsSameTeam(m_team))
// 		return nullptr;
// 	return piece;
// }

void Player::UndoMove(const Move& move, char old_cell)
{
	char piece = GetCopiedCell(move.target.x, move.target.y);
	GetCopiedCell(move.target.x, move.target.y) = old_cell;
	GetCopiedCell(move.origin.x, move.origin.y) = piece;
}

char Player::TestMove(const Move& move)
{
	const pt2di& s = move.origin;
	const pt2di& e = move.target;
	char piece = GetCopiedCell(s.x, s.y);
	char old_cell = GetCopiedCell(e.x, e.y);
	GetCopiedCell(s.x, s.y) = COPY_EMPTY_CELL;
	GetCopiedCell(e.x, e.y) = piece;
	return old_cell;
}

bool Player::IsCheck() const
{
	pt2di kingPosition = m_king->Pos();
	return m_pBoard->IsCellAttacked(kingPosition, m_team);
}

bool Player::IsHypotheticalCheck() const
{
	pt2di king_pos;
	int team_mod = m_king->GetMod(m_team);
	int enemy_team_mod = -team_mod;
	std::vector<pt2di> enemy_pieces_pos;
	for (int i = 0; i < m_pBoard->GetWidth(); i++)
	{
		for (int j = 0; j < m_pBoard->GetHeight(); j++)
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

	// utility lambdas
	auto inBounds = [&](const pt2di& c)
	{
		return (c.x >= 0 && c.x < m_pBoard->GetWidth() && c.y >= 0 && c.y < m_pBoard->GetHeight());
	};
	auto isEmptyCell = [&](const pt2di& c)
	{
		return (GetCopiedCell(c.x, c.y) == COPY_EMPTY_CELL);
	};

	// queen is not included because we can check for threats from a queen with the bishop and the rook.
	std::vector<vector2d<int>> dir_bishop = { {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
	std::vector<vector2d<int>> dir_rook = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
	std::vector<vector2d<int>> dir_knight = { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} };
	std::vector<vector2d<int>> dir_king = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1} };
	std::vector<vector2d<int>> dir_pawn = { {-1, -1}, {1, -1} }; // pawns can only attack in diagonals

	// check if there's a bishop or a queen
	for (auto& d : dir_bishop)
	{
		bool blocked = false;
		for (int i = 1; i < 9; i++)
		{
			pt2di cell = king_pos + d * i;
			if (!inBounds(cell))
				break;

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
			if (blocked)
				break;
		}
	}

	// check if there's a rook or a queen
	for (auto& d : dir_rook)
	{
		bool blocked = false;
		for (int i = 1; i < 9; i++)
		{
			pt2di cell = king_pos + d * i;
			if (!inBounds(cell))
				break;

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
			if (blocked)
				break;
		}
	}

	// check if there's a knight
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

	// check if there's a king
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

	// check if there's a pawn
	for (auto& d : dir_pawn)
	{
		// TODO: need to multiply the y by -1 depending on which team we're on
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
		if (move.origin == m.origin && move.target == m.target)
		{
			return true;
		}
	}
	return false;
}

void Player::CopyBoard()
{
	for (int i = 0; i < m_pBoard->GetWidth(); ++i)
	{
		for (int j = 0; j < m_pBoard->GetHeight(); ++j)
		{
			Piece* p = m_pBoard->GetPieceAtCell({ i, j });
			if (p != nullptr)
			{
				if (!p->IsCaptured())
				{
					GetCopiedCell(i, j) = p->Type() * p->GetMod(m_team);
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
	// std::cout << "calculating moves\n";
	for (const auto& piece : m_pieces)
	{
		if (piece->IsCaptured())
			continue;

		// make sure we copy the available moves because we're resetting it after.
		// We're resetting the moves because we don't want to display the moves
		// that aren't valid to the player
		std::vector<pt2di> availableMoves = piece->GetAvailableMoves();
		piece->ResetAvailableMoves();
		for (const auto& availableMove : availableMoves)
		{
			// verify the move won't put yourself in check
			Move move = { availableMove, piece->Pos() };
			char old_cell = TestMove(move);
			// if the move takes the king out of check
			if (!IsHypotheticalCheck())
			{
				piece->AddAvailableMove(move.target);
				possibleMoves.push_back(move);
			}
			// undo the move
			UndoMove(move, old_cell);
			// UndoMove(move, old_cell);
		}
	}
	return possibleMoves;
}

void Player::SelectPiece(const pt2di& position)
{
	Piece* piece = m_pBoard->GetPieceAtCell(position);
	if (piece == nullptr)
	{
		m_selectedPiece = nullptr;
		return;
	}
	if (piece->IsSameTeam(m_team))
		m_selectedPiece = piece;
}

MoveInfo Player::MoveSelectedPiece(const pt2di& position)
{
	if (!IsMoveLegal(m_selectedPiece, position))
		return { MoveInfo::INVALID_MOVE };

	// we don't want to move the piece if the move is invalid
	if (!m_selectedPiece->IsMoveValid(position))
		return { MoveInfo::INVALID_MOVE };

	m_selectedPiece->Move(position); // move the piece
	m_selectedPiece = nullptr; // deselect the piece
	
	return { MoveInfo::NONE };
}

bool Player::IsMoveLegal(const Piece* piece, const pt2di& target) const
{
	for (const auto& move : m_legalMoves)
	{
		if (move.target == target && move.origin == piece->Pos())
			return true;
	}
	return false;
}


std::string Player::GetBoardAsString() const
{
	std::stringstream board;
	const Board* pBoard = m_pBoard;
	for (int j = 0; j < m_pBoard->GetHeight(); ++j)
	{
		for (int i = 0; i < m_pBoard->GetWidth(); ++i)
		{
			const Piece* piece = pBoard->GetPieceAtCell({i, j});
			if (piece == nullptr)
			{
				// empty cell
				board << '.';
			}
			else if (piece->IsSameTeam(m_team))
			{
				board << to_uppercase(piece->Type());
			}
			else
			{
				board << to_lowercase(piece->Type());
			}

		}
		board << '\n';
	}

	return board.str();
}