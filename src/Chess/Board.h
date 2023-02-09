#pragma once

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define SQUARE_CELLS false

#define EVEN_CELL_COLOR BLACK
#define ODD_CELL_COLOR WHITE

#include "Cell.h"
#include "Renderer2D/Renderer2D.h"

class Board
{
public:
	Board(int width, int height);
	~Board();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	Cell* GetCell(point2d<int> pos);

	point2d<int> WindowToBoardCoordinates(point2d<int> windowCoordinates, point2d<int> cellDims) const;

	void DrawCells(const Renderer2D* renderer) const;
	void DrawSelectedCell(const Renderer2D* renderer, point2d<int> cellPos, int width) const;
	void HighlightCell(const Renderer2D* renderer, point2d<int> cellPos, point2d<int> padding = {0, 0}, const Color& color = YELLOW) const;
	
private:
	Cell* m_cells;
	int m_width;
	int m_height;
};