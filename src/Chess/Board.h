#pragma once

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define SQUARE_CELLS false

#define EVEN_CELL_COLOR BLACK
#define ODD_CELL_COLOR WHITE

#include "Cell.h"
#include "Renderer2D/Renderer2D.h"

#include <string>

class Board
{
public:
	Board(int width, int height);
	~Board();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	Cell* GetCell(const pt2di& pos);

	void ResetGuard();

	point2d<int> WindowToBoardCoordinates(const pt2di& windowCoordinates, const vec2di& cellDims) const;
	bool IsPositionValid(const pt2di& position) const;

	void DrawCells(const Renderer2D* renderer) const;
	void DrawSelectedCell(const Renderer2D* renderer, const pt2di& cellPos, int width) const;
	void HighlightCell(const Renderer2D* renderer, const pt2di& cellPos, const pt2di& padding = {0, 0}, const Color& color = YELLOW) const;
	
	std::string GetBoardCoordinates(const pt2di& position) const;

private:
	Cell* m_cells;
	int m_width;
	int m_height;
};