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
	Board(const Board& other);
	~Board();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	Cell* GetCell(const pt2di& pos);
	void SetCellDim(const vec2di& dim);
	const vec2di& GetCellDim() const { return m_cellDim; }

	void AttackCell(const pt2di& position, TEAM team);
	void ResetAttack();

	pt2di WindowToBoardCoordinates(const pt2di& windowCoordinates, const vec2di& cellDims) const;
	pt2di BoardToWindowCoordinates(const pt2di& windowCoordinates, const vec2di& cellDims) const;

	pt2di WindowToBoardCoordinates(const pt2di& windowCoordinates) const;
	pt2di BoardToWindowCoordinates(const pt2di& windowCoordinates) const;

	bool IsPositionValid(const pt2di& position) const;

	void DrawCells(const Renderer2D* renderer) const;
	void DrawSelectedCell(const Renderer2D* renderer, const pt2di& cellPos, int width, const Color& color) const;
	void HighlightCell(const Renderer2D* renderer, const pt2di& cellPos, const pt2di& padding = {0, 0}, const Color& color = YELLOW) const;
	void DrawCellLabels(const Renderer2D* renderer, const Color& color) const;
	
	std::string GetBoardCoordinates(const pt2di& position) const;

private:
	Cell* m_cells;
	int m_width;
	int m_height;

	vec2di m_cellDim;
};