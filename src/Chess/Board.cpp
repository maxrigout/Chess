#include "Board.h"
#include "Piece.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Core/Logger.h"

#include <iostream>
#include <sstream>

// #define DRAW_DEBUG
#define USE_BOARD_SPRITE
#define USE_SELECTED_CELL_SPRITE
#define USE_HOVERED_CELL_SPRITE

bool isUpperCase(char c)
{
	return c >= 'A' && c <= 'Z';
}

bool isLowerCase(char c)
{
	return c >= 'a' && c <= 'z';
}

char toUpperCase(char c)
{
	if (isUpperCase(c))
		return c;
	return (char)((int)'A' + (int)c - (int)'a');
}

char toLowerCase(char c)
{
	if (isLowerCase(c))
		return c;
	return (char)((int)'a' + (int)c - (int)'A');
}

Board::Board(int width, int height)
{
	m_width = width;
	m_height = height;
	m_cells = new Cell[m_width * m_height];
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			m_cells[y * m_width + x].m_coordinates = { x, y };
		}
	}
	// m_benchCursor = { m_width - 1, 0 };
	m_benchCursor = { 0, 0 };
	CreatePlayer1Pieces();
	CreatePlayer2Pieces();
}

Board::Board(const Board& other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_screenDim = other.m_screenDim;
	m_cellDim = other.m_cellDim;
	m_cells = new Cell[m_width * m_height];
	for (int i = 0; i < m_width * m_height; ++i)
	{
		m_cells[i].m_coordinates = other.m_cells[i].m_coordinates;
		m_cells[i].m_attackedBy = other.m_cells[i].m_attackedBy;
		Piece* otherPiece = other.m_cells[i].m_piece;
		if (otherPiece != nullptr)
		{
			Piece* piece = CopyPiece(otherPiece);
			PlacePiece(piece);
			AddPieceToTeam(piece, otherPiece->Team());
		}
	}
}

Board::~Board()
{
	DeletePieces(m_player2Pieces);
	DeletePieces(m_player1Pieces);
	delete[] m_cells;
}

Cell* Board::GetCell(const pt2di& pos)
{
	if (pos.x < 0 || pos.x >= m_width)
		return nullptr;
	if (pos.y < 0 || pos.y >= m_height)
		return nullptr;
	return &m_cells[pos.y * m_width + pos.x];
}

const Cell* Board::GetCell(const pt2di& pos) const
{
	if (pos.x < 0 || pos.x >= m_width)
		return nullptr;
	if (pos.y < 0 || pos.y >= m_height)
		return nullptr;
	return &m_cells[pos.y * m_width + pos.x];
}

void Board::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}

pt2di Board::WindowToBoardCoordinates(const pt2di& windowCoordinates, const vec2di& cellDims) const
{
	return pt2di{ (windowCoordinates.x - m_margin.w) / cellDims.w, (windowCoordinates.y - m_margin.h) / cellDims.h };
}

pt2di Board::BoardToWindowCoordinates(const pt2di& boardCoordinates, const vec2di& cellDims) const
{
	return (pt2di{ boardCoordinates.x * cellDims.w, boardCoordinates.y * cellDims.h } + m_margin);
}

pt2di Board::WindowToBoardCoordinates(const pt2di& windowCoordinates) const
{
	return WindowToBoardCoordinates(windowCoordinates, m_cellDim);
}

pt2di Board::BoardToWindowCoordinates(const pt2di& boardCoordinates) const
{
	return BoardToWindowCoordinates(boardCoordinates, m_cellDim);
}

bool Board::IsPositionValid(const pt2di& position) const
{
	if (position.x < 0 || position.y < 0)
		return false;
	if (position.x >= m_width || position.y >= m_height)
		return false;
	return true;
}

void Board::AttackCell(const pt2di& position, TEAM team)
{
	if (!IsPositionValid(position))
		return;
	GetCell(position)->Attack(team);
}

void Board::ResetAttack()
{
	for (int i = 0; i < m_width * m_height; ++i)
	{
		m_cells[i].ResetAttacked();
	}
}

void Board::DrawBoard(const Renderer2D* renderer) const
{
#ifdef USE_BOARD_SPRITE
	DrawBoardTextured(renderer);
#else
	DrawBoardBasic(renderer);
#endif
}

void Board::DrawBoardTextured(const Renderer2D* renderer) const
{
	const vec2di& windowDim = renderer->GetViewPortDim();
	int sz = std::min(windowDim.x, windowDim.y);
	renderer->DrawSprite({ 0, 0 }, { sz, sz }, "board");
#ifdef DRAW_DEBUG
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			if (m_cells[j * m_width + i].m_attackedBy != TEAM::NONE)
				HighlightCell(renderer, {i, j}, {15, 15}, FG_RED);
		}
	}
#endif
}

void Board::DrawBoardBasic(const Renderer2D* renderer) const
{
	vec2di screen = renderer->GetViewPortDim();
	vec2di cell = renderer->GetCellDim();

	pt2di p1 = { 0, 0 };
	pt2di p2 = { 0, screen.h };
	vec2di dir = { cell.w, 0 };
	for (int j = 0; j < m_width; j++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		renderer->DrawLine(p1, p2, BLACK);
	}
	p1 = { 0, 0 };
	p2 = { screen.w, 0 };
	dir = { 0, cell.h };
	for (int i = 0; i < m_height; i++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		renderer->DrawLine(p1, p2, BLACK);
	}

	// alternate between black and white cells
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			Color color = ((i + j) % 2 == 0) ? EVEN_CELL_COLOR : ODD_CELL_COLOR;
			renderer->FillRect({ i * cell.w, j * cell.h }, cell, color);
#ifdef DRAW_DEBUG
			// if we want to draw the cells that are attacked
			if (m_cells[j * m_width + i].m_attackedBy != TEAM::NONE)
				HighlightCell(renderer, { i, j }, { 15, 15 }, FG_RED);
			// 	DrawSelectedCell(renderer, { i, j }, 5, DARK_BLUE);
#endif
		}
	}
}

void Board::DrawCellLabels(const Renderer2D* renderer, const Color& color) const
{
	renderer->DrawText(pt2di{ 0, 0 } + m_margin, { 15, 15 }, GetBoardCoordinates({ 0, 0 }), color);
	for (int i = 1; i < m_width; ++i)
	{
		// draw the letter
		renderer->DrawText(pt2di{ renderer->GetCellDim().x * i , 0 } + m_margin, { 8, 15 }, { (char)('a' + i), '\0' }, color);
	}
	for (int j = 1; j < m_height; ++j)
	{
		// draw the number
		renderer->DrawText(pt2di{ 0 , j * renderer->GetCellDim().y } + m_margin, { 8, 15 }, std::to_string(m_height - j), color);
	}
}

void Board::DrawSelectedCell(const Renderer2D* renderer, const pt2di& cellPos, int width, const Color& color) const
{
	if (!IsPositionValid(cellPos))
		return;

	vec2di cell = renderer->GetCellDim();

#ifdef USE_SELECTED_CELL_SPRITE
	renderer->DrawSprite({ cellPos.x * cell.w + m_margin.w, cellPos.y * cell.h + m_margin.h },
						 { cell.w, cell.h }, "selectedCell");
#else
	// option 1
	// draw 4 rects
	/*
	__________
	|___Top___|
	|  |   |R |
	|L_|___|__|
	|___Bot___|

	*/

	float aspectRatio = (float)cell.h / (float)cell.w;
	int height = width * aspectRatio;

	pt2di cellTopLeft{ cellPos.x * cell.w, cellPos.y * cell.h };
	pt2di cellTopRight{ (cellPos.x + 1) * cell.w, cellPos.y * cell.h };

	pt2di topTopLeft{ cellTopLeft.x, cellTopLeft.y };
	vec2di topDim{ cell.w, height };

	pt2di leftTopLeft{ cellTopLeft.x, cellTopLeft.y + height };
	vec2di leftDim{ width, cell.h - (2 * height) };

	pt2di rightTopLeft{ cellTopRight.x - width, cellTopRight.y + height };
	vec2di rightDim{ width, cell.h - (2 * height) };

	pt2di bottomTopLeft{ cellTopLeft.x, cellTopLeft.y + cell.h - height };
	vec2di bottomDim{ cell.w, height };

	renderer->FillRect(topTopLeft + m_margin, topDim, color); // top rect
	renderer->FillRect(leftTopLeft + m_margin, leftDim, color); // left rect
	renderer->FillRect(rightTopLeft + m_margin, rightDim, color); // right rect
	renderer->FillRect(bottomTopLeft + m_margin, bottomDim, color); // bottom rect

	// option 2
	// draw smaller and smaller rect
	// for (int i = 0; i < width; i++)
	// {
	// 	SDL_SetRenderDrawColor(renderer->renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);
	// 	SDL_Rect rect = {cellPos.x * renderer->cell.w + i, cellPos.y * renderer->cell.h + i, renderer->cell.w - 2 * i, renderer->cell.h - 2 * i};
	// 	SDL_RenderDrawRect(renderer->renderer, &rect);
	// }
#endif
}

void Board::HighlightCell(const Renderer2D* renderer, const pt2di& cellPos, const pt2di& padding, const Color& color) const
{
	if (!IsPositionValid(cellPos))
		return;

	vec2di cell = renderer->GetCellDim();

#ifdef USE_HOVERED_CELL_SPRITE
	renderer->DrawSprite({ cellPos.x * cell.w + padding.x + m_margin.w, cellPos.y * cell.h + padding.y + m_margin.h },
						 { cell.w - 2 * padding.x, cell.h - 2 * padding.y }, "hoveredCell");
#else
	renderer->FillRect(	{ cellPos.x * cell.w + padding.x + m_margin.w, cellPos.y * cell.h + padding.y + m_margin.h },
						{ cell.w - 2 * padding.x, cell.h - 2 * padding.y },
						color);
#endif
}

std::string Board::GetBoardCoordinates(const pt2di& position) const
{
	if (!IsPositionValid(position))
		return "";

	char column = 'a' + position.x;
	std::string boardCoords = column + std::to_string(m_height - position.y);
	return boardCoords;
}

std::string Board::ToString() const
{
	std::string out("+");
	for (int i = 0; i < m_width; i++)
	{
		out += '-';
	}
	out += "+\n";
	for (int j = 0; j < m_height; j++)
	{
		out += "|";
		for (int i = 0; i < m_width; i++)
		{
			const Cell* cell = GetCell({ i, j });
			const Piece* piece = cell->m_piece;
			if (piece == nullptr)
				out += COPY_EMPTY_CELL;
			else
			{
				if (piece->Team() == TEAM::ONE)
					out += toUpperCase(piece->Type());
				else
					out += toLowerCase(piece->Type());
			}
		}
		out += "|\n";
	}
	out += '+';
	for (int i = 0; i < m_width; i++)
	{
		out += '-';
	}
	out += "+";
	return out;
}

Piece* Board::GetPieceAtCell(const pt2di& pos) const
{
	const Cell* cell = GetCell(pos);
	if (cell == nullptr)
		return nullptr;
	return cell->m_piece;
}

void Board::PlacePiece(Piece* piece)
{
	Cell* cell = GetCell(piece->Pos());
	if (cell == nullptr)
		return;
	cell->m_piece = piece;
}

void Board::MovePiece(Piece* piece, const pt2di& destination, bool hasSideEffect)
{
	// we have to move the piece
	// add the move to the move stack
	Cell* currentCell = GetCell(piece->Pos());
	Cell* destinationCell = GetCell(destination);
	if (currentCell == nullptr || destinationCell == nullptr)
		return;

	destinationCell->m_piece = piece;
	currentCell->m_piece = nullptr;
	MoveEvent move{ piece, destination, piece->Pos(), EventType::Move, piece->IsFirstMove(), hasSideEffect };
	m_moveStack.push(move);
}

void Board::Castle(Piece* rook, const pt2di& target)
{
	Cell* currentRookCell = GetCell(rook->Pos());
	Cell* destinationRookCell = GetCell(target);
	// potentially unecessary
	if (currentRookCell == nullptr || destinationRookCell == nullptr)
		return;

	destinationRookCell->m_piece = rook;
	currentRookCell->m_piece = nullptr;
	MoveEvent rookMove{ rook, target, rook->Pos(), EventType::Castle, rook->IsFirstMove(), true };
	m_moveStack.push(rookMove);
}

bool Board::CaptureLocation(const pt2di& location)
{
	// we have to move the piece
	// add the move to the move stack
	Cell* cell = GetCell(location);
	if (cell == nullptr)
		return false;

	Piece* capturedPiece = cell->m_piece;
	if (capturedPiece == nullptr)
		return false;

	capturedPiece->GetCaptured(GetNextBenchLocation());
	MoveEvent event{ capturedPiece, location, capturedPiece->Pos(), EventType::GetCaptured, capturedPiece->IsFirstMove(), false };
	m_moveStack.push(event);
	cell->m_piece = nullptr;
	return true;
}

bool Board::DoesCellHavePiece(const pt2di& pos) const
{
	const Cell* cell = GetCell(pos);
	if (cell == nullptr)
		return false;
	return cell->m_piece != nullptr;
}

bool Board::IsCellAttacked(const pt2di& pos, TEAM team) const
{
	const Cell* cell = GetCell(pos);
	if (cell == nullptr)
		return false;
	return cell->m_attackedBy != TEAM::NONE && cell->m_attackedBy != team;
}

void Board::ResetCellsAttack()
{
	for (int i = 0; i < m_width * m_height; ++i)
	{
		m_cells[i].m_attackedBy = TEAM::NONE;
	}
}

bool Board::UndoMove(int i)
{
	if (i)
		LOG_DEBUG("undo move " + std::to_string(i));
	if (m_moveStack.empty())
	{
		LOG_DEBUG("move stack is empty!");
		return false;
	}
	MoveEvent lastEvent = m_moveStack.top();
	m_moveStack.pop();
	Piece* piece = lastEvent.piece;

	Cell* currentCell = GetCell(piece->Pos());
	Cell* destinationCell = GetCell(lastEvent.origin);

	currentCell->m_piece = nullptr;
	destinationCell->m_piece = piece;

	lastEvent.piece->UndoMove(lastEvent.origin, lastEvent.wasFirstMove);
	if (lastEvent.eventType == EventType::GetCaptured)
	{
		// if a piece was captured, the bench location would have been updated
		GetPreviousBenchLocation();
	}

	if (lastEvent.hasSideEffect)
		UndoMove(i++);
	
	return true;
}

pt2di Board::GetNextBenchLocation()
{
	int leftMostSlot = m_width * m_cellDim.w + 2 * m_margin.w;
	if (m_benchCursor.x == 0)
	{
		// initialize
		// emove the cell width because if we're initializeing, 
		// we want the x position to be equal to the leftMostSlot.
		m_benchCursor.x = leftMostSlot - m_cellDim.w;
	}
	m_benchCursor.x += m_cellDim.w;
	// this checks if the slot will fit (in width) inside the window.
	if (m_benchCursor.x + m_cellDim.w > m_screenDim.w)
	{
		// we go to the next row
		m_benchCursor.y += m_cellDim.h;
		m_benchCursor.x = leftMostSlot;
	}
	return m_benchCursor;
}

pt2di Board::GetPreviousBenchLocation()
{
	int leftMostSlot = m_width * m_cellDim.w + 2 * m_margin.w;
	int extraRoomPx = m_screenDim.w - leftMostSlot; // extra room in px.. needs to be a multiple of m_cellDim.w
	int numberExtraSlots = extraRoomPx / m_cellDim.w; // number of horizontal slots for the bench
	int rightMostSlot = leftMostSlot + m_cellDim.w * (numberExtraSlots - 1);
	m_benchCursor.x -= m_cellDim.w;
	// this checks if we bled into the board
	if (m_benchCursor.x < leftMostSlot)
	{
		// we want to go up a row
		m_benchCursor.y -= m_cellDim.h;
		m_benchCursor.x = rightMostSlot;
	}
	return m_benchCursor;
}

std::vector<Piece*> Board::CreatePieces(TEAM team)
{
	std::vector<Piece*> pieces;

	int pawn_row = 6;
	int king_row = 7;
	PAWN_DIRECTION pawn_direction = PAWN_DIRECTION::UP;
	if (team == TEAM::TWO)
	{
		pawn_row = 1;
		king_row = 0;
		pawn_direction = PAWN_DIRECTION::DOWN;
	}

	Piece *r1, *r2, *b1, *b2, *k1, *k2, *q, *k;
	r1 = 	new Rook	(this, team, { 0, king_row });
	k1 = 	new Knight	(this, team, { 1, king_row });
	b1 = 	new Bishop	(this, team, { 2, king_row });
	q = 	new	Queen	(this, team, { 3, king_row });
	k = 	new King	(this, team, { 4, king_row });
	b2 = 	new Bishop	(this, team, { 5, king_row });
	k2 = 	new Knight	(this, team, { 6, king_row });
	r2 = 	new Rook	(this, team, { 7, king_row });

	pieces = { r1, k1, b1, q, k, b2, k2, r2 };

	for (int i = 0; i < GetWidth(); ++i)
	{
		pieces.push_back(new Pawn(this, team, { i, pawn_row }, pawn_direction));
	}

	for (const auto& piece : pieces)
		PlacePiece(piece);

	return pieces;
}

void Board::DeletePieces(const std::vector<Piece*>& pieces)
{
	for (const auto piece : pieces)
		delete piece;
}

void Board::CreatePlayer1Pieces()
{
	m_player1Pieces = CreatePieces(TEAM::ONE);
}

void Board::CreatePlayer2Pieces()
{
	m_player2Pieces = CreatePieces(TEAM::TWO);
}

std::string Board::MoveStackToString() const
{
	auto s = m_moveStack;
	std::stringstream ss;
	const int maxSz = 200;
	char data[maxSz] = {0};
	while (!s.empty())
	{
		auto m = s.top();
		int charWritten = 0;
		if (m.eventType == EventType::GetCaptured)
		{
			charWritten = snprintf(data, maxSz, "(captured %c%d: %s) < ", m.piece->Type(), (m.piece->Team() == TEAM::TWO) + 1, GetBoardCoordinates(m.origin).c_str());
			// ss << " < ( captured " << m.piece->Type() << ": " + GetBoardCoordinates(m.origin) << ")";
		}
		else if (m.eventType == EventType::Castle)
		{
			charWritten = snprintf(data, maxSz, "(castle %c%d) < ", m.piece->Type(), (m.piece->Team() == TEAM::TWO) + 1);
		}
		else if (m.eventType == EventType::Move)
		{
			charWritten = snprintf(data, maxSz,"(move %c%d: %s -> %s) < ", m.piece->Type(), (m.piece->Team() == TEAM::TWO) + 1, GetBoardCoordinates(m.origin).c_str(), GetBoardCoordinates(m.target).c_str());
		}
		data[charWritten - 3] = 0;
		ss << data;
		s.pop();
	}
	return ss.str();
}

const std::vector<Piece*>& Board::GetPlayerPieces(TEAM team) const
{
	if (team == TEAM::ONE)
		return m_player1Pieces;

	if (team == TEAM::TWO)
		return m_player2Pieces;

	throw "invalid team!";
}

Piece* Board::CopyPiece(Piece* piece)
{
	Piece* copiedPiece = nullptr;
	switch (piece->Type())
	{
		case 'P': copiedPiece = new Pawn(this, piece->Team(), piece->Pos(), PAWN_DIRECTION::UP); break;
		case 'R': copiedPiece = new Rook(this, piece->Team(), piece->Pos()); break;
		case 'H': copiedPiece = new Knight(this, piece->Team(), piece->Pos()); break;
		case 'B': copiedPiece = new Bishop(this, piece->Team(), piece->Pos()); break;
		case 'Q': copiedPiece = new Queen(this, piece->Team(), piece->Pos()); break;
		case 'K': copiedPiece = new King(this, piece->Team(), piece->Pos()); break;
		default: return nullptr;
	}
	copiedPiece->m_boardPosition = piece->m_boardPosition;
	copiedPiece->m_moves = piece->m_moves;
	copiedPiece->m_availableMoves = piece->m_availableMoves;
	copiedPiece->m_team = piece->m_team;
	copiedPiece->m_teamColor = piece->m_teamColor;
	copiedPiece->m_pieceType = piece->m_pieceType;
	copiedPiece->m_isCaptured = piece->m_isCaptured;
	copiedPiece->m_isFirstMove = piece->m_isFirstMove;
	copiedPiece->m_isMovesCalculated = false;
	copiedPiece->m_id = piece->m_id;
	copiedPiece->m_screenPosition = piece->m_screenPosition;
	copiedPiece->m_targetScreenPosition = piece->m_targetScreenPosition;
	copiedPiece->m_speed = piece->m_speed;
	return copiedPiece;
}

void Board::AddPieceToTeam(Piece* piece, TEAM team)
{
	if (team == TEAM::ONE)
		m_player1Pieces.push_back(piece);
	if (team == TEAM::TWO)
		m_player2Pieces.push_back(piece);
}

void Board::CreatePieces()
{
	DeletePieces(m_player1Pieces);
	DeletePieces(m_player2Pieces);
	CreatePlayer1Pieces();
	CreatePlayer2Pieces();
}

MoveEvent Board::GetLastMoveEvent() const
{
	return m_moveStack.top();
}

Move Board::GetLastMove() const
{
	return {m_moveStack.top().origin, m_moveStack.top().target};
}

// TODO move this inside a GUI manager class
void Board::ShowHourglass(const std::string& additionalMessage) const
{

}

void Board::UpdateHourglass(float dt)
{

}

void Board::ResetHourglass() const
{

}

void Board::DrawHourglass(const Renderer2D* renderer) const
{

}
