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

Cell* Board::GetCell(point2d<int> pos)
{
	if (pos.x < 0 || pos.x >= m_width)
		return nullptr;
	if (pos.y < 0 || pos.y >= m_height)
		return nullptr;
	return &m_cells[pos.y * m_width + pos.x];
}

point2d<int> Board::WindowToBoardCoordinates(point2d<int> windowCoordinates, vector2d<int> cellDims) const
{
	return point2d<int>{windowCoordinates.x / cellDims.w, windowCoordinates.y / cellDims.h};
}

bool Board::IsPositionValid(point2d<int> position) const
{
	if (position.x < 0 || position.y < 0)
		return false;
	if (position.x >= m_width || position.y >= m_height)
		return false;
	return true;
}

void Board::DrawCells(const Renderer2D* renderer) const
{
	vector2d<int> screen = renderer->GetWindowDim();
	vector2d<int> cell = renderer->GetCellDim();

	point2d<int> p1 = { 0, 0 };
	point2d<int> p2 = { 0, screen.h };
	vector2d<int> dir = { cell.w, 0 };
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
		}
	}
}

void Board::DrawSelectedCell(const Renderer2D* renderer, point2d<int> cellPos, int width) const
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
	vector2d<int> cell = renderer->GetCellDim();

	float aspectRatio = (float)cell.h / (float)cell.w;
	int height = width * aspectRatio;

	point2d<int> cellTopLeft{ cellPos.x * cell.w, cellPos.y * cell.h };
	point2d<int> cellTopRight{ (cellPos.x + 1) * cell.w, cellPos.y * cell.h };

	point2d<int> topTopLeft{ cellTopLeft.x, cellTopLeft.y };
	vector2d<int> topDim{ cell.w, height };

	point2d<int> leftTopLeft{ cellTopLeft.x, cellTopLeft.y + height };
	vector2d<int> leftDim{ width, cell.h - (2 * height) };

	point2d<int> rightTopLeft{ cellTopRight.x - width, cellTopRight.y + height };
	vector2d<int> rightDim{ width, cell.h - (2 * height) };

	point2d<int> bottomTopLeft{ cellTopLeft.x, cellTopLeft.y + cell.h - height };
	vector2d<int> bottomDim{ cell.w, height };

	renderer->FillRect(topTopLeft, topDim, DARK_GREEN); // top rect
	renderer->FillRect(leftTopLeft, leftDim, DARK_GREEN); // left rect
	renderer->FillRect(rightTopLeft, rightDim, DARK_GREEN); // right rect
	renderer->FillRect(bottomTopLeft, bottomDim, DARK_GREEN); // bottom rect

	// option 2
	// draw smaller and smaller rect
	// for (int i = 0; i < width; i++)
	// {
	// 	SDL_SetRenderDrawColor(renderer->renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);
	// 	SDL_Rect rect = {cellPos.x * renderer->cell.w + i, cellPos.y * renderer->cell.h + i, renderer->cell.w - 2 * i, renderer->cell.h - 2 * i};
	// 	SDL_RenderDrawRect(renderer->renderer, &rect);
	// }
}

void Board::HighlightCell(const Renderer2D* renderer, point2d<int> cellPos, point2d<int> padding, const Color& color) const
{
	if (!IsPositionValid(cellPos))
		return;

	vector2d<int> cell = renderer->GetCellDim();

	renderer->FillRect(	{ cellPos.x * cell.w + padding.x, cellPos.y * cell.h + padding.y },
						{ cell.w - 2 * padding.x, cell.h - 2 * padding.y },
						color);
}