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

point2d<int> Board::WindowToBoardCoordinates(point2d<int> windowCoordinates, point2d<int> cellDims) const
{
	return point2d<int>{windowCoordinates.x / cellDims.x, windowCoordinates.y / cellDims.y};
}

void Board::DrawCells(const Renderer2D* renderer) const
{
	// SDL_Rect rect;
	// SDL_RenderGetViewport(renderer->renderer, &rect);
	// int screenWidth = rect.w;
	// int screenHeight = rect.h;
	int screenWidth = renderer->windowWidth;
	int screenHeight = renderer->windowHeight;
	int cellWidth = renderer->cellWidth;
	int cellHeight = renderer->cellHeight;

	point2d<int> p1 = { 0, 0 };
	point2d<int> p2 = { 0, screenHeight };
	vector2d<int> dir = { cellWidth, 0 };
	for (int j = 0; j < m_width; j++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		SDL_RenderDrawLine(renderer->renderer, p1.x, p1.y, p2.x, p2.y);
	}
	p1 = { 0, 0 };
	p2 = { screenWidth, 0 };
	dir = { 0, cellHeight };
	for (int i = 0; i < m_height; i++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		SDL_RenderDrawLine(renderer->renderer, p1.x, p1.y, p2.x, p2.y);
	}

	// alternate between black and white cells
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			Color color = ((i + j) % 2 == 0) ? EVEN_CELL_COLOR : ODD_CELL_COLOR;
			SDL_Rect rect{i * cellWidth, j * cellHeight, cellWidth, cellHeight};
			SDL_SetRenderDrawColor(renderer->renderer, color.r, color.g, color.b, color.a);
			SDL_RenderFillRect(renderer->renderer, &rect);
		}
	}
}

void Board::DrawSelectedCell(const Renderer2D* renderer, point2d<int> cellPos, int width) const
{
	// option 1
	// draw the highlight color
	// draw the 4 rects
	/*
	__________
	|___Top___|
	|  |   |R |
	|L_|___|__|
	|___Bot___|

	*/
	float aspectRatio = (float)renderer->cellHeight / (float)renderer->cellWidth;
	int height = width * aspectRatio;
	point2d<int> topLeft{ cellPos.x * renderer->cellWidth, cellPos.y * renderer->cellHeight };
	point2d<int> topRight{ (cellPos.x + 1) * renderer->cellWidth, cellPos.y * renderer->cellHeight };

	SDL_SetRenderDrawColor(renderer->renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);

	SDL_Rect topRect{ topLeft.x,
		topLeft.y,
		renderer->cellWidth,
		height
	};
	SDL_Rect leftRect{ topLeft.x,
		topLeft.y + height,
		width,
		renderer->cellHeight - (2 * height)
	};
	SDL_Rect rightRect{ topRight.x - width,
		topRight.y + height,
		width,
		renderer->cellHeight - (2 * height)
	};
	SDL_Rect bottomRect{ topLeft.x,
		topLeft.y + renderer->cellHeight - height,
		renderer->cellWidth,height
	};

	SDL_RenderFillRect(renderer->renderer, &topRect);
	SDL_RenderFillRect(renderer->renderer, &leftRect);
	SDL_RenderFillRect(renderer->renderer, &rightRect);
	SDL_RenderFillRect(renderer->renderer, &bottomRect);


	// option 2
	// draw smaller and smaller rect
	// for (int i = 0; i < width; i++)
	// {
	// 	SDL_SetRenderDrawColor(renderer->renderer, DARK_GREEN.r, DARK_GREEN.g, DARK_GREEN.b, DARK_GREEN.a);
	// 	SDL_Rect rect = {cellPos.x * renderer->cellWidth + i, cellPos.y * renderer->cellHeight + i, renderer->cellWidth - 2 * i, renderer->cellHeight - 2 * i};
	// 	SDL_RenderDrawRect(renderer->renderer, &rect);
	// }
}

void Board::HighlightCell(const Renderer2D* renderer, point2d<int> cellPos, point2d<int> padding, const Color& color) const
{
	if (cellPos.x < 0 || cellPos.x >= m_width)
		return;
	if (cellPos.y < 0 || cellPos.y >= m_height)
		return;
	SDL_Rect rect{cellPos.x * renderer->cellWidth + padding.x,
		cellPos.y * renderer->cellHeight + padding.y,
		renderer->cellWidth - 2 * padding.x,
		renderer->cellHeight - 2 * padding.y
	};
	SDL_SetRenderDrawColor(renderer->renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer->renderer, &rect);
	// FillRect(cell_x * cellWidth + pad_x, cell_y * cellHeight + pad_y, cellWidth - 2 * pad_x, cellHeight - 2 * pad_y, color);
}