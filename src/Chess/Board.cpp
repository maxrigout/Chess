#include "Board.h"

#include <iostream>
Board::Board(int width, int height)
{
	m_width = width;
	m_height = height;
	m_cells = new Cell[width * height];
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			m_cells[y * width + x].coordinates = { x, y };
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
			// if (m_cells[j * m_width + i].IsGuarded())
			// 	HighlightCell(renderer, {i, j}, {15, 15}, RED);
			// 	DrawSelectedCell(renderer, {i, j}, 5, DARK_BLUE);
		}
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