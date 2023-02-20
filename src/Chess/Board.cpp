#include "Board.h"
#include "Piece.h"

#include <iostream>

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
	m_benchCursor = { m_width - 1, 0 };
}

Board::Board(const Board& other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_cells = new Cell[m_width * m_height];
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			m_cells[y * m_width + x] = Cell(other.m_cells[y * m_width + x]);
		}
	}
}

Board::~Board()
{
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
	return pt2di{windowCoordinates.x / cellDims.w, windowCoordinates.y / cellDims.h};
}

pt2di Board::BoardToWindowCoordinates(const pt2di& boardCoordinates, const vec2di& cellDims) const
{
	return pt2di{boardCoordinates.x * cellDims.w, boardCoordinates.y * cellDims.h};
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

void Board::DrawCells(const Renderer2D* renderer) const
{
	vec2di screen = renderer->GetWindowDim();
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
			// if we want to draw the cells that are attacked
			// if (m_cells[j * m_width + i].IsAttacked())
			// 	HighlightCell(renderer, {i, j}, {15, 15}, RED);
			// 	DrawSelectedCell(renderer, {i, j}, 5, DARK_BLUE);
		}
	}
}

void Board::DrawCellLabels(const Renderer2D* renderer, const Color& color) const
{
	renderer->DrawText({ 0 , 0 }, { 15, 15 }, GetBoardCoordinates({ 0, 0 }), color);
	for (int i = 1; i < m_width; ++i)
	{
		// draw the letter
		renderer->DrawText({ renderer->GetCellDim().x * i , 0 }, { 8, 15 }, { (char)('a' + i), '\0' }, color);
	}
	for (int j = 1; j < m_height; ++j)
	{
		// draw the number
		renderer->DrawText({ 0 , j * renderer->GetCellDim().y }, { 8, 15 }, std::to_string(m_height - j), color);
	}
}

void Board::DrawSelectedCell(const Renderer2D* renderer, const pt2di& cellPos, int width, const Color& color) const
{
	if (!IsPositionValid(cellPos))
		return;

	// option 1
	// draw 4 rects
	/*
	__________
	|___Top___|
	|  |   |R |
	|L_|___|__|
	|___Bot___|

	*/
	vec2di cell = renderer->GetCellDim();

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

	renderer->FillRect(topTopLeft, topDim, color); // top rect
	renderer->FillRect(leftTopLeft, leftDim, color); // left rect
	renderer->FillRect(rightTopLeft, rightDim, color); // right rect
	renderer->FillRect(bottomTopLeft, bottomDim, color); // bottom rect

	// option 2
	// draw smaller and smaller rect
	// for (int i = 0; i < width; i++)
	// {
	// 	SDL_SetRenderDrawColor(renderer->renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);
	// 	SDL_Rect rect = {cellPos.x * renderer->cell.w + i, cellPos.y * renderer->cell.h + i, renderer->cell.w - 2 * i, renderer->cell.h - 2 * i};
	// 	SDL_RenderDrawRect(renderer->renderer, &rect);
	// }
}

void Board::HighlightCell(const Renderer2D* renderer, const pt2di& cellPos, const pt2di& padding, const Color& color) const
{
	if (!IsPositionValid(cellPos))
		return;

	vec2di cell = renderer->GetCellDim();

	renderer->FillRect(	{ cellPos.x * cell.w + padding.x, cellPos.y * cell.h + padding.y },
						{ cell.w - 2 * padding.x, cell.h - 2 * padding.y },
						color);
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
	out += "+\n";
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

void Board::MovePiece(Piece* piece, const pt2di& destination)
{
	// we have to move the piece
	// add the move to the move stack
	Cell* currentCell = GetCell(piece->Pos());
	Cell* destinationCell = GetCell(destination);
	if (currentCell == nullptr || destinationCell == nullptr)
		return;

	destinationCell->m_piece = piece;
	currentCell->m_piece = nullptr;
	MoveEvent move{ piece, destination, piece->Pos(), EventType::Move, piece->IsFirstMove() };
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
	MoveEvent rookMove{ rook, target, rook->Pos(), EventType::Castle, rook->IsFirstMove() };
	m_moveStack.push(rookMove);
}

void Board::CaptureLocation(const pt2di& location)
{
	// we have to move the piece
	// add the move to the move stack
	Cell* cell = GetCell(location);
	if (cell == nullptr)
		return;

	Piece* capturedPiece = cell->m_piece;
	if (capturedPiece == nullptr)
		return;
	capturedPiece->GetCaptured(GetNextBenchLocation());
	MoveEvent event{ capturedPiece, location, capturedPiece->Pos(), EventType::GetCaptured };
	m_moveStack.push(event);
	cell->m_piece = nullptr;
}

bool Board::DoesCellHavePiece(const pt2di& pos) const
{
	const Cell* cell = GetCell(pos);
	if (cell == nullptr)
		return false;
	return cell->m_piece != nullptr;
}

bool Board::IsCellAttacked(const pt2di& pos) const
{
	const Cell* cell = GetCell(pos);
	if (cell == nullptr)
		return false;
	return cell->m_attackedBy != TEAM::NONE;
}

void Board::ResetCellsAttack()
{
	for (int i = 0; i < m_width * m_height; ++i)
	{
		m_cells[i].m_attackedBy = TEAM::NONE;
	}
}

void Board::TestMove()
{

}

void Board::UndoMove()
{
	MoveEvent lastEvent = m_moveStack.top();
	Piece* piece = lastEvent.piece;
	m_moveStack.pop();

	Cell* currentCell = GetCell(piece->Pos());
	Cell* destinationCell = GetCell(lastEvent.origin);

	currentCell->m_piece = nullptr;
	destinationCell->m_piece = piece;

	lastEvent.piece->UndoMove(lastEvent.origin, lastEvent.wasFirstMove);
	switch (lastEvent.eventType)
	{
	case EventType::Castle:
		UndoMove();
		break;
	case EventType::GetCaptured:
		break;
	}

	// hack because the GetCaptured event happens before the Move event
	if (m_moveStack.top().eventType == EventType::GetCaptured)
	{
		// if a piece was captured, the bench location would have been updated
		GetPreviousBenchLocation();
		UndoMove();
	}
}

pt2di Board::GetNextBenchLocation()
{
	m_benchCursor.x++;
	// 
	if ((m_benchCursor.x + 1) * m_cellDim.w > m_screenDim.w)
	{
		m_benchCursor.y++;
		m_benchCursor.x = m_width;
	}
	return { m_benchCursor.x * m_cellDim.w, m_benchCursor.y * m_cellDim.h };
}

pt2di Board::GetPreviousBenchLocation()
{
	m_benchCursor.x--;
	if (m_benchCursor.x < m_width)
	{
		m_benchCursor.y--;
		// int extraRoom = m_screenDim.w - (m_cellDim.w * m_width); // in px
		int numberExtraCells = m_screenDim.w / m_width - m_cellDim.w;
		m_benchCursor.x = m_width + numberExtraCells - 1;
	}
	return { m_benchCursor.x * m_cellDim.w, m_benchCursor.y * m_cellDim.h };
}