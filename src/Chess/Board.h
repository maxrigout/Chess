#pragma once

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define SQUARE_CELLS false

#define EVEN_CELL_COLOR BLACK
#define ODD_CELL_COLOR WHITE

#include "Common.h"
#include "Cell.h"
#include "Renderer2D/Renderer2D.h"

#include <string>
#include <stack>

class Board
{
public:
	Board(int width, int height);
	Board(const Board& other);
	~Board();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	void SetCellDim(const vec2di& dim);
	const vec2di& GetCellDim() const { return m_cellDim; }

	Piece* GetPieceAtCell(const pt2di& pos) const;
	void PlacePiece(Piece* piece);
	void MovePiece(Piece* piece, const pt2di& target);
	void CaptureLocation(const pt2di& target);
	bool DoesCellHavePiece(const pt2di& pos) const;
	bool IsCellAttacked(const pt2di& pos) const;
	void ResetCellsAttack();

	void TestMove();
	void UndoMove();

	pt2di GetNextBenchLocation();

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

	std::string ToString() const;

	void SetScreenDim(const vec2di& dims) { m_screenDim = dims; }

private:
	Cell* GetCell(const pt2di& pos);
	const Cell* GetCell(const pt2di& pos) const;

	Cell* m_cells;
	int m_width;
	int m_height;

	vec2di m_cellDim;
	vec2di m_screenDim;

	std::stack<MoveEvent> m_moveStack;

	pt2di m_benchCursor;
};