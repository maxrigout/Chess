#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <vector>

//using namespace std;

class Chess;

typedef enum TEAM
{
	ONE,
	TWO,
	NONE,
};

struct MoveInfo
{
	enum REASON
	{
		NONE = 0,
		BLOCKED = 1,
		SAME_TEAM = 2,
		ALREADY_OCCUPIED = 4,
		KING_GUARDED = 8,
		EMPTY_CELL = 16,
		CELL_GUARDED = 32,
		CAPTURABLE = 64,
		UNCAPTURABLE = 128,
		INVALID_CELL = 256,
		INVALID_MOVE = 512,
	};

	int reason;
}MainInfo;

template <typename T>
struct vector2d
{
	vector2d() { x = 0; y = 0; }
	vector2d(T v1, T v2)
	{
		x = v1;
		y = v2;
	}
	vector2d(const vector2d<T>& p)
	{
		x = p.x;
		y = p.y;
	}

	vector2d<T>& operator=(const vector2d<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	bool operator==(const vector2d<T>& v) const
	{
		return (v.x == x && v.y == y);
	}
	vector2d<T> operator+(const vector2d<T>& v) const
	{
		return vector2d<T>(x + v.x, y + v.y);
	}
	vector2d<T> operator-(const vector2d<T>& v) const
	{
		return vector2d<T>(x - v.x, y - v.y);
	}
	T operator*(const vector2d<T>& rhs) const
	{
		return (x * rhs.x + y * rhs.y);
	}
	T Norm() const { return sqrt((*this) * (*this)); }
	vector2d<T> operator*(T s) const
	{
		return vector2d<T>(x * s, y * s);
	}
	vector2d<T> operator/(T s) const
	{
		return vector2d<T>(x / s, y / s);
	}

	//vector2d<T>& Normalize()
	//{
	//	T norm = (*this) * (*this);
	//	*this = *this / norm;
	// return *this;
	//}

	bool IsColinear(const vector2d<T>& v) const
	{
		return ((x * v.y - y * v.x) == 0);
	}

	bool IsPositiveMultiple(const vector2d<T>& v) const
	{
		bool isColinear = IsColinear(v);
		T k = 0;
		if (isColinear) 
		{
			if (x != 0)
			{
				k = v.x / x;
			}
			else
			{
				k = v.y / y;
			}

			if (k == 0)
			{
				if (v.x != 0)
				{
					k = x / v.x;
				}
				else
				{
					k = y / v.y;
				}
			}
		}
		return (isColinear && k > 0);
	}

	T x;
	T y;
};

template <typename T>
struct point2d
{
	point2d()
	{
		x = 0;
		y = 0;
	}
	point2d(T v1, T v2)
	{
		x = v1;
		y = v2;
	}
	point2d(const point2d<T>& p)
	{
		x = p.x;
		y = p.y;
	}

	bool operator==(const point2d<T>& rhs) const
	{
		return (x == rhs.x && y == rhs.y);
	}
	point2d<T> operator=(const point2d<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	point2d<T> operator+(const vector2d<T>& v) const
	{
		return point2d<T>(x + v.x, y + v.y);
	}
	vector2d<T> operator-(const point2d<T>& p) const
	{
		return vector2d<T>(x - p.x, y - p.y);
	}
	point2d<T> operator-(const vector2d<T>& p) const
	{
		return point2d<T>(x - p.x, y - p.y);
	}

	T x;
	T y;
};

class chesspiece
{
public:
	chesspiece(Chess* game, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 }, char t = 'N', std::vector<vector2d<int>> moves = { {0, 0} }, bool s = false);
	virtual bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const;
	virtual void Move(const point2d<int>& target);
	virtual void DrawMoves() const;
	void DrawYourself() const;
	bool IsSameTeam(TEAM t) const { return (team == t); }
	bool HasMoved() const { return !first_move; }
	TEAM Team() const { return team; }
	bool IsCaptured() const { return isCaptured; }
	bool CapturePiece();
	point2d<int> Pos() const { return pos; }
	char Type() const { return piece_type; }
	void GuardCells();
	virtual bool CanGuard(const point2d<int>& target);

protected:
	point2d<int> pos;
	bool move_scaling;
	std::vector<vector2d<int>> moves;
	Chess* pGame;
	TEAM team;
	olc::Pixel team_color;
	char piece_type;
	bool isCaptured;
	bool first_move;
};

class pawn : public chesspiece
{
public:
	pawn(Chess* game, TEAM nTeam = TEAM::NONE, point2d<int> p = { -1, -1 });
	bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const override;
	bool CanGuard(const point2d<int>& target) override;
};
class tower : public chesspiece
{
public:
	tower(Chess* game, TEAM nTeam = NONE, point2d<int> p = { -1, -1 });
};
class bishop : public chesspiece
{
public:
	bishop(Chess* game, TEAM nTeam = NONE, point2d<int> p = { -1, -1 });
};
class knight : public chesspiece
{
public:
	knight(Chess* game, TEAM nTeam = NONE, point2d<int> p = { -1, -1 });
};
class queen : public chesspiece
{
public:
	queen(Chess* game, TEAM nTeam = NONE, point2d<int> p = { -1, -1 });
};
class king : public chesspiece
{
public:
	king(Chess* game, TEAM nTeam = NONE, point2d<int> p = { -1, -1 });
	bool IsMoveValid(const point2d<int>& target, MoveInfo& info = MainInfo) const override;
	void DrawMoves() const override;
	void Move(const point2d<int>& target) override;
	bool CanGuard(const point2d<int>& target) override;
	bool Check();
	bool CheckMate();
};

class chesscell
{
public:
	chesscell(point2d<int> p);
	chesscell(int x, int y);
	~chesscell();
	bool PlacePiece(chesspiece *p);
	bool RemovePiece();
	bool CaptureCell();
	bool HasPiece() { return (piece != nullptr); }
	bool IsEmpty() { return (piece == nullptr); }
	bool IsSameTeam(TEAM t);
	chesspiece* GetPiece() { return piece; }
	point2d<int> GetCoordinates() const { return coordinates; }
	bool IsGuarded() { return guarded != NONE; }
	void Guard(TEAM t) { guarded = t; }
	void ResetGuarded() { guarded = TEAM::NONE; }

private:
	point2d<int> coordinates;
	chesspiece* piece;
	bool hasPiece;
	TEAM guarded;
};

class Chess : public olc::PixelGameEngine
{
private:
	// board size
	int boardWidth, boardHeight;

	// cell size
	int cellWidth, cellHeight;

	std::vector<chesspiece*>* activePlayer;
	int player = 0;
	TEAM activeTeam = NONE;

	bool mheld = false;

	std::vector<chesspiece*> player1;
	std::vector<chesspiece*> player2;

	chesscell* selectedCell;
	chesspiece* selectedPiece;

	chesscell*** board;

public:
	Chess();
	~Chess();

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	bool OnUserDestroy() override;

	void DrawCells();
	void DrawSelectedCell(int width = 3);

	point2d<int> GetCell(int x, int y) { return { x / cellWidth, y / cellHeight }; }
	point2d<int> GetCell(point2d<int> p) { return { GetCell(p.x, p.y) }; }

	chesscell* GetChessCell(int x, int y);
	chesscell* GetChessCell(point2d<int> pos);

	void HighlightCell(int cellx, int celly, int pad_x = 0, int pad_y = 0, olc::Pixel color = olc::YELLOW);
	void HighlightCell(point2d<int> cell, int pad_x = 0, int pad_y = 0, olc::Pixel color = olc::YELLOW);

	void SelectCell(point2d<int> cell) { selectedCell = board[cell.x][cell.y]; }

	void SwitchPlayers();

	int GetBoardWidth() { return boardWidth; }
	int GetBoardHeight() { return boardHeight; }

public:
	void DrawDisk(point2d<int> pos, olc::Pixel color);
	void DrawLabel(point2d<int> pos, std::string label, olc::Pixel color);
};

int main(int argc, char* argv[]) {

	Chess game;

	if (game.Construct(800, 600, 1, 1))
	{
		game.Start();
	}

	return 0;
}


Chess::Chess()
{
	sAppName = "Chess";
}
Chess::~Chess()
{
}
bool Chess::OnUserCreate()
{

	boardWidth = 8;
	boardHeight = 8;

	bool square_cells = false; // if we want the cells to be square

	if (square_cells)
	{
		int val = (ScreenWidth() / boardWidth > ScreenHeight() / boardHeight) ? ScreenHeight() / boardHeight : ScreenWidth() / boardWidth;
		cellWidth = val;
		cellHeight = val;
	}
	else
	{
		cellWidth = ScreenWidth() / boardWidth;
		cellHeight = ScreenHeight() / boardHeight;
	}

	board = new chesscell**[boardWidth];
	for (int x = 0; x < boardWidth; ++x)
	{
		board[x] = new chesscell*[boardHeight];
		for (int y = 0; y < boardWidth; ++y)
		{
			board[x][y] = new chesscell({ x, y });
		}
	}

	TEAM team = TEAM::ONE;
	chesspiece* t1, * t2, * b1, * b2, * h1, * h2, * q, * k;
	t1	=	new	tower	(this, team, { 0, 7 });
	h1	=	new	knight	(this, team, { 1, 7 });
	b1	=	new	bishop	(this, team, { 2, 7 });
	q	=	new	queen	(this, team, { 3, 7 });
	k	=	new	king	(this, team, { 4, 7 });
	b2	=	new	bishop	(this, team, { 5, 7 });
	h2	=	new	knight	(this, team, { 6, 7 });
	t2	=	new	tower	(this, team, { 7, 7 });
	player1 = { t1, h1, b1, q, k, b2, h2, t2 };


	for (int i = 0; i < 8; i++)
	{
		chesspiece* p = new pawn(this, team, { i, 6 });
		player1.push_back(p);
	}

	for (auto piece : player1)
	{
		int x = piece->Pos().x;
		int y = piece->Pos().y;
		board[x][y]->PlacePiece(piece);
	}

	team = TEAM::TWO;
	t1	= new	tower	(this, team, { 0, 0 });
	h1	= new	knight	(this, team, { 1, 0 });
	b1	= new	bishop	(this, team, { 2, 0 });
	q	= new	queen	(this, team, { 3, 0 });
	k	= new	king	(this, team, { 4, 0 });
	b2	= new	bishop	(this, team, { 5, 0 });
	h2	= new	knight	(this, team, { 6, 0 });
	t2	= new	tower	(this, team, { 7, 0 });
	player2 = { t1, h1, b1, q, k, b2, h2, t2 };

	for (int i = 0; i < 8; i++)
	{
		chesspiece* p = new pawn(this, team, { i, 1 });
		player2.push_back(p);
	}

	for (auto piece : player2)
	{
		int x = piece->Pos().x;
		int y = piece->Pos().y;
		board[x][y]->PlacePiece(piece);
	}

	SelectCell({ 0, 0 });
	activePlayer = &player1;
	activeTeam = activePlayer->at(0)->Team();

	return true;
}
bool Chess::OnUserUpdate(float fElapsedTime)
{
	// Get mouse information
	point2d<int> mouse = { GetMouseX(), GetMouseY() };
	point2d<int> mouseCell = GetCell(mouse); // cell underneath the mouse cursor


	// Draw
	Clear(olc::BLACK); // Clear the screen
	DrawCells(); // Draw the board
	if (selectedPiece != nullptr)
	{
		selectedPiece->DrawMoves(); // draw the selected piece's moves
	}
	HighlightCell(mouseCell); // highlight the cell under the mouse cursor
	DrawSelectedCell(); // highlight the selected cell

	if (GetMouse(0).bPressed) // if the left mouse button is pressed
	{
		SelectCell(mouseCell);
		if (selectedPiece == nullptr && selectedCell->HasPiece()) { // if we don't have any piece selected
			if (selectedCell->GetPiece()->IsSameTeam(activeTeam) && !selectedCell->GetPiece()->IsCaptured())
			{
				selectedPiece = selectedCell->GetPiece();
			}
		}
		else if (selectedPiece != nullptr && selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move is valid
		{
			selectedPiece->Move(mouseCell); // move the piece
			selectedPiece = nullptr; // de-select the piece
			SwitchPlayers(); // end the current player's turn and allow the other player to play
		}
		else if (selectedPiece != nullptr && !selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move isn't valid
		{
			selectedPiece = nullptr;
			if (selectedCell->HasPiece()) { // check to see if we can select another piece
				if (selectedCell->GetPiece()->IsSameTeam(activeTeam) && !selectedCell->GetPiece()->IsCaptured())
				{
					selectedPiece = selectedCell->GetPiece();
				}
			}
		}
	}

	// draw the player 1's pieces
	for (auto p : player1)
	{
		p->DrawYourself();
	}

	// draw the player 2's pieces
	for (auto p : player2)
	{
		p->DrawYourself();
	}

	return true;
}
bool Chess::OnUserDestroy()
{
	for (auto p : player1)
	{
		delete p;
	}

	for (auto p : player2)
	{
		delete p;
	}
	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			delete board[x][y];
		}
		delete[] board[x];
	}
	delete[] board;

	return true;
}

void Chess::DrawCells()
{
	point2d<int> p1 = { 0, 0 };
	point2d<int> p2 = { 0, ScreenHeight() };
	vector2d<int> dir = { cellWidth, 0 };
	for (int j = 0; j < boardWidth; j++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		DrawLine(p1.x, p1.y, p2.x, p2.y);
	}

	p1 = { 0, 0 };
	p2 = { ScreenWidth(), 0 };
	dir = { 0, cellHeight };
	for (int i = 0; i < boardHeight; i++)
	{
		p1 = p1 + dir;
		p2 = p2 + dir;
		DrawLine(p1.x, p1.y, p2.x, p2.y);
	}

	// alternate between black and white cells
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			olc::Pixel color = ((i + j) % 2 == 0) ? olc::BLACK : olc::WHITE;
			FillRect(i * cellWidth, j * cellHeight, cellWidth, cellHeight, color);
		}
	}
}
void Chess::DrawSelectedCell(int width)
{
	// draws a rectangle with a border of width pixels
	for (int i = 0; i < width; i++)
	{
		point2d<int> p = selectedCell->GetCoordinates();
		DrawRect(p.x * cellWidth + i, p.y * cellHeight + i, cellWidth - 2 * i, cellHeight - 2 * i, olc::DARK_GREEN);
	}

}

chesscell* Chess::GetChessCell(int x, int y)
{
	if (x >= 0 && x < boardWidth && y >= 0 && y < boardHeight) { return board[x][y]; }
	return nullptr;
}
chesscell* Chess::GetChessCell(point2d<int> pos)
{ 
	return GetChessCell(pos.x, pos.y);
}

void Chess::HighlightCell(int cell_x, int cell_y, int pad_x, int pad_y, olc::Pixel color)
{
	FillRect(cell_x * cellWidth + pad_x, cell_y * cellHeight + pad_y, cellWidth - 2 * pad_x, cellHeight - 2 * pad_y, color);
}
void Chess::HighlightCell(point2d<int> cell, int pad_x, int pad_y, olc::Pixel color)
{
	HighlightCell(cell.x, cell.y, pad_x, pad_y, color);
}

void Chess::DrawDisk(point2d<int> pos, olc::Pixel color)
{
	FillCircle(pos.x * cellWidth + (cellWidth / 2), pos.y * cellHeight + (cellHeight / 2), 20, color);
}
void Chess::DrawLabel(point2d<int> pos, std::string label, olc::Pixel color)
{
	int offx = -3;
	int offy = -3;
	DrawString(pos.x * cellWidth + (cellWidth / 2) + offx, pos.y * cellHeight + (cellHeight / 2) + offy, label, color);
}
void Chess::SwitchPlayers()
{
	// reset the guarded cells
	for (int j = 0; j < boardHeight; ++j) 
	{
		for (int i = 0; i < boardWidth; ++i)
		{
			board[i][j]->ResetGuarded();
		}
	}
	// mark the cells that are guarded by the previous player
	for (auto p : *activePlayer) 
	{
		p->GuardCells();
	}

	// should check for check / checkmate here

	// switch players
	player = (player + 1) % 2;
	if (player == 0)
	{
		activePlayer = &player1;
	}
	else
	{
		activePlayer = &player2;
	}
	activeTeam = activePlayer->at(0)->Team();
}

chesspiece::chesspiece(Chess* game, TEAM nTeam, point2d<int> p, char t, std::vector<vector2d<int>> m, bool s)
{ 
	first_move = true;
	isCaptured = false;
	pGame = game; 
	team = nTeam;
	pos = p;
	piece_type = t;
	moves = m;
	move_scaling = s;
	switch (team)
	{
	case TEAM::ONE: team_color = olc::DARK_GREY; break;
	case TEAM::TWO: team_color = olc::DARK_YELLOW; break;
	default: team_color = olc::GREEN; break;
	}
}
bool chesspiece::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	/* * STEP 1: CHECK TO SEE IF FIRST MOVE (FOR KING AND PAWN)
	   * STEP 2: CHECK TO SEE IF THERE IS A PIECE AT THAT LOCATION
	   * STEP 2.1: CHECK TO SEE IF IT'S AN ENEMY PIECE (YES CONTINUE, NO RETURN FALSE)
	   * STEP 3: CHECK TO SEE IF YOU CAN GET TO THE TARGET POSITION
	*/

	// if we're checking for an out of bounds cell
	if (target.x > pGame->GetBoardWidth() || target.y > pGame->GetBoardHeight() || target.x < 0 || target.y < 0) 
	{ 
		return false; 
	}

	vector2d<int> stepdir;
	point2d<int> step(pos);
	vector2d<int> movevect(target - pos);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	if (!move_scaling)
	{
		for (auto m : moves)
		{
			if (movevect == m)
			{
				chesscell* target_cell = pGame->GetChessCell(pos + m);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (auto m : moves)
		{
			if (movevect.IsPositiveMultiple(m))
			{
				if (m.x != 0)
				{
					nSteps = movevect.x / m.x;
				}
				else
				{
					nSteps = movevect.y / m.y;
				}
				stepdir = m;
				found_dir = true;
				break;
			}
		}

		// check each cells between current pos and target pos
		if (found_dir) {
			std::vector<point2d<int>> path;
			bool blocked = false;

			path.resize(nSteps);

			for (int i = 1; i < nSteps + 1; ++i)
			{
				path[i - 1] = pos + (stepdir * i);
			}
			for (auto tile : path)
			{
				chesscell* target_cell = pGame->GetChessCell(tile);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
				{
					return false;
				}
				else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team))
				{
					blocked = true;
				}
				else if (blocked)
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
bool chesspiece::CanGuard(const point2d<int>& target)
{
	// if we're checking for an out of bounds cell
	if (target.x >= pGame->GetBoardWidth() || target.y >= pGame->GetBoardHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}

	vector2d<int> stepdir;
	point2d<int> step(pos);
	vector2d<int> movevect(target - pos);
	bool found_dir = false;
	int nSteps = 0;

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	if (!move_scaling)
	{
		for (auto m : moves)
		{
			if (movevect == m)
			{
				chesscell* target_cell = pGame->GetChessCell(pos + m);
				if (target_cell == nullptr)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (auto m : moves)
		{
			if (movevect.IsPositiveMultiple(m))
			{
				if (m.x != 0)
				{
					nSteps = movevect.x / m.x;
				}
				else
				{
					nSteps = movevect.y / m.y;
				}
				stepdir = m;
				found_dir = true;
				break;
			}
		}

		// check each cells between current pos and target pos
		if (found_dir) {
			std::vector<point2d<int>> path;
			bool blocked = false;
			path.resize(nSteps);

			for (int i = 1; i < nSteps + 1; ++i)
			{
				path[i - 1] = pos + (stepdir * i);
			}
			for (auto tile : path)
			{
				chesscell* target_cell = pGame->GetChessCell(tile);
				if (target_cell == nullptr)
				{
					return false;
				}
				else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team) && target_cell->GetPiece()->Type() != 'K')
				{
					blocked = true;
				}
				else if (blocked)
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
void chesspiece::DrawMoves() const
{
	int padx = 2;
	int pady = 2;

	if (!move_scaling)
	{
		for (auto& m : moves)
		{
			if (IsMoveValid(pos + m)) 
			{
				pGame->HighlightCell(pos + m, padx, pady, olc::DARK_GREEN);
			}
		}
	}
	else
	{
		for (auto& m : moves)
		{
			point2d<int> target_cell;

			for (int i = 1; i < 8; ++i)
			{
				target_cell = pos + (m * i);
				if (IsMoveValid(target_cell))
				{
					pGame->HighlightCell(target_cell, padx, pady, olc::DARK_GREEN);
				}
				else
				{
					break;
				}
			}
		}
	}
}
void chesspiece::GuardCells()
{
	if (!move_scaling)
	{
		for (auto& m : moves)
		{
			chesscell* cell = pGame->GetChessCell(pos + m);
			if (cell != nullptr && CanGuard(pos + m))
			{
				cell->Guard(team);
			}
		}
	}
	else
	{
		for (auto& m : moves)
		{
			point2d<int> target_cell;

			for (int i = 1; i < 8; ++i)
			{
				target_cell = pos + (m * i);
				if (CanGuard(target_cell))
				{
					pGame->GetChessCell(target_cell)->Guard(team);
				}
				else
				{
					break;
				}
			}
		}
	}
}
void chesspiece::DrawYourself() const
{
	if (!isCaptured) {
		pGame->DrawDisk(pos, team_color);
		pGame->DrawLabel(pos, { piece_type, '\0' }, olc::WHITE);
	}
}
void chesspiece::Move(const point2d<int>& target)
{
	chesscell* oldcell = pGame->GetChessCell(pos);
	chesscell* newcell = pGame->GetChessCell(target);
	if (oldcell != nullptr && newcell != nullptr)
	{
		pos = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		std::cout << "Moved " << piece_type << std::endl;
	}
	if (first_move)
	{
		first_move = false;
	}
}
bool chesspiece::CapturePiece()
{
	if (!isCaptured)
	{
		isCaptured = true;
		return true;
	}
	return false;
}

pawn::pawn(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'P', { {0, 0} }, false)
{
	int i = 1;
	switch (team)
	{
	case TEAM::TWO: i = 1; break;
	case TEAM::ONE: i = -1; break;
	default: break;
	}

	vector2d<int> move_front(0, i);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));
	moves = { move_front, move_front2, capture_left, capture_right };

}
bool pawn::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	vector2d<int> move_vect(target - pos);
	vector2d<int> move_front(moves[0]);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));

	// if we're checking for an out of bounds cell
	if (target.x > pGame->GetBoardWidth() || target.y > pGame->GetBoardHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	else if (move_vect == move_front)
	{
		chesscell* target_cell = pGame->GetChessCell(pos + move_vect);
		if (target_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (move_vect == move_front2 && first_move)
	{
		chesscell* target_cell = pGame->GetChessCell(pos + move_vect);
		chesscell* intermediate_cell = pGame->GetChessCell(pos + move_front);
		if (target_cell == nullptr || intermediate_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece() || intermediate_cell->HasPiece())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (move_vect == capture_left || move_vect == capture_right)
	{
		chesscell* target_cell = pGame->GetChessCell(pos + move_vect);
		if (target_cell == nullptr)
		{
			return false;
		}
		else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool pawn::CanGuard(const point2d<int>& target)
{
	vector2d<int> move_vect(target - pos);
	vector2d<int> move_front(moves[0]);
	vector2d<int> move_front2(move_front + move_front);
	vector2d<int> capture_left(move_front + vector2d<int>(-1, 0));
	vector2d<int> capture_right(move_front + vector2d<int>(1, 0));

	// if we're checking for an out of bounds cell
	if (target.x > pGame->GetBoardWidth() || target.y > pGame->GetBoardHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	else if (move_vect == move_front || (move_vect == move_front2 && first_move))
	{
		return false;
	}
	else if (move_vect == capture_left || move_vect == capture_right)
	{
		return true;
	}
	return false;
}

tower::tower(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'T', { { 0, 1 } , {1, 0}, {0, -1 }, {-1, 0} }, true) {}
bishop::bishop(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'B', { { 1, 1 } , {1, -1}, {-1, -1}, {-1, 1} }, true) {}
knight::knight(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'H', { {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1} }, false) {}
queen::queen(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'Q', { {1, 1}, {1, -1}, {0, 1}, {1, 0}, {0, -1 }, {-1, 0}, {-1, -1}, {-1, 1} }, true) {}

king::king(Chess* game, TEAM nTeam, point2d<int> p) : chesspiece(game, nTeam, p, 'K', { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, -1}, {0, 1} }, false){}
bool king::IsMoveValid(const point2d<int>& target, MoveInfo& info) const
{
	// if we're checking for an out of bounds cell
	if (target.x > pGame->GetBoardWidth() || target.y > pGame->GetBoardHeight() || target.x < 0 || target.y < 0)
	{
		info.reason = MoveInfo::INVALID_CELL;
		return false;
	}
	vector2d<int> movevect(target - pos);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		info.reason = MoveInfo::INVALID_MOVE;
		return false;
	}

	if (first_move) // check castle
	{
		if (movevect == vector2d<int>(2, 0))
		{
			chesspiece* rightTower = pGame->GetChessCell(pos + vector2d<int>(3, 0))->GetPiece();
			if (rightTower != nullptr)
			{
				chesscell* c1 = pGame->GetChessCell(pos + vector2d<int>(2, 0));
				chesscell* c2 = pGame->GetChessCell(pos + vector2d<int>(1, 0));
				if (!rightTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty())
				{
					return true;
				}
			}
			info.reason = MoveInfo::INVALID_MOVE;
			return false;
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			chesspiece* leftTower = pGame->GetChessCell(pos + vector2d<int>(-4, 0))->GetPiece();
			if (leftTower != nullptr)
			{
				chesscell* c1 = pGame->GetChessCell(pos + vector2d<int>(-3, 0));
				chesscell* c2 = pGame->GetChessCell(pos + vector2d<int>(-2, 0));
				chesscell* c3 = pGame->GetChessCell(pos + vector2d<int>(-1, 0));
				if (!leftTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty() && c3->IsEmpty())
				{
					return true;
				}
			}
			info.reason = MoveInfo::INVALID_MOVE;
			return false;
		}
	}

	for (auto& m1 : moves)
	{
		if (movevect == m1)
		{
			chesscell* target_cell = pGame->GetChessCell(pos + m1);
			if (target_cell == nullptr)
			{
				info.reason = MoveInfo::INVALID_CELL;
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
			{
				info.reason = MoveInfo::ALREADY_OCCUPIED | MoveInfo::SAME_TEAM;
				return false;
			}
			else if (target_cell->HasPiece() && !target_cell->IsSameTeam(team) && target_cell->IsGuarded())
			{
				info.reason = MoveInfo::CELL_GUARDED;
				return false;
			}
			else if (!target_cell->IsGuarded())
			{
				info.reason = MoveInfo::NONE;
				return true;
			}
			break;
		}
	}

	info.reason = MoveInfo::INVALID_MOVE;
	return false;
}
void king::DrawMoves() const
{
	int padx = 2;
	int pady = 2;

	for (auto& m : moves)
	{
		if (IsMoveValid(pos + m))
		{
			pGame->HighlightCell(pos + m, padx, pady, olc::DARK_GREEN);
		}
	}

	if (first_move) // check for castle
	{

		if (IsMoveValid(pos + vector2d<int>(2, 0)))
		{
			pGame->HighlightCell(pos + vector2d<int>(2, 0), padx, pady, olc::DARK_GREEN);
		}
		if (IsMoveValid(pos + vector2d<int>(-3, 0)))
		{
			pGame->HighlightCell(pos + vector2d<int>(-3, 0), padx, pady, olc::DARK_GREEN);
		}

		//chesspiece* rightTower = pGame->GetChessCell(pos + vector2d<int>(3, 0))->GetPiece();
		//if (rightTower != nullptr)
		//{
		//	chesscell* c1 = pGame->GetChessCell(pos + vector2d<int>(2, 0));
		//	chesscell* c2 = pGame->GetChessCell(pos + vector2d<int>(1, 0));
		//	if (!rightTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty())
		//	{
		//		pGame->HighlightCell(pos + vector2d<int>(2, 0), padx, pady, olc::DARK_GREEN);
		//	}
		//}
		//chesspiece* leftTower = pGame->GetChessCell(pos + vector2d<int>(-4, 0))->GetPiece();
		//if (leftTower != nullptr)
		//{
		//	chesscell* c1 = pGame->GetChessCell(pos + vector2d<int>(-3, 0));
		//	chesscell* c2 = pGame->GetChessCell(pos + vector2d<int>(-2, 0));
		//	chesscell* c3 = pGame->GetChessCell(pos + vector2d<int>(-1, 0));
		//	if (!leftTower->HasMoved() && c1->IsEmpty() && c2->IsEmpty() && c3->IsEmpty())
		//	{
		//		pGame->HighlightCell(pos + vector2d<int>(-3, 0), padx, pady, olc::DARK_GREEN);
		//	}
		//}
	}
}
void king::Move(const point2d<int>& target)
{
	chesscell* oldcell = pGame->GetChessCell(pos);
	chesscell* newcell = pGame->GetChessCell(target);
	vector2d<int> movevect(target - pos);
	if (first_move)
	{
		first_move = false;
		if (movevect == vector2d<int>(2, 0))
		{
			chesspiece* rightTower = pGame->GetChessCell(pos + vector2d<int>(3, 0))->GetPiece();
			rightTower->Move(rightTower->Pos() + vector2d<int>(-2, 0));
		}
		else if (movevect == vector2d<int>(-3, 0))
		{
			chesspiece* leftTower = pGame->GetChessCell(pos + vector2d<int>(-4, 0))->GetPiece();
			leftTower->Move(leftTower->Pos() + vector2d<int>(2, 0));
		}
	}
	if (oldcell != nullptr && newcell != nullptr)
	{
		pos = target;
		oldcell->RemovePiece();
		newcell->CaptureCell();
		newcell->PlacePiece(this);
		std::cout << "Moved " << piece_type << std::endl;
	}
}
bool king::CanGuard(const point2d<int>& target)
{
	// if we're checking for an out of bounds cell
	if (target.x > pGame->GetBoardWidth() || target.y > pGame->GetBoardHeight() || target.x < 0 || target.y < 0)
	{
		return false;
	}
	vector2d<int> movevect(target - pos);

	// if we're checking against the current piece position
	if (movevect.x == 0 && movevect.y == 0)
	{
		return false;
	}

	for (auto& m1 : moves)
	{
		if (movevect == m1)
		{
			chesscell* target_cell = pGame->GetChessCell(pos + m1);
			if (target_cell == nullptr)
			{
				return false;
			}
			else if (target_cell->HasPiece() && target_cell->IsSameTeam(team))
			{
				return true;
			}
			break;
		}
	}
	return false;
}
bool king::Check()
{
	return pGame->GetChessCell(pos)->IsGuarded();
}
bool king::CheckMate()
{
	if (Check())
	{
		for (auto& m : moves)
		{
			if (IsMoveValid(pos+m) && !pGame->GetChessCell(pos + m)->IsGuarded())
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

chesscell::chesscell(point2d<int> p) : coordinates(p) 
{
	piece = nullptr; 
	hasPiece = false; 
	guarded = TEAM::NONE;
	//std::cout << "created cell at (" << p.x << ", " << p.y << ")\n";
}
chesscell::chesscell(int x, int y) 
{ 
	coordinates.x = x; 
	coordinates.y = y;
	piece = nullptr;
	hasPiece = false;
	guarded = TEAM::NONE;
}
chesscell::~chesscell()
{
	//std::cout << "deleted cell at (" << coordinates.x << ", " << coordinates.y << ")\n";
}
bool chesscell::PlacePiece(chesspiece* p)
{
	if (piece == nullptr)
	{
		piece = p;
		hasPiece = true;
		return true;
	}
	return false;
}
bool chesscell::RemovePiece()
{
	if (piece == nullptr)
	{
		return false;
	}
	piece = nullptr;
	hasPiece = false;
	return true;
}
bool chesscell::IsSameTeam(TEAM t) 
{
	if (piece == nullptr)
	{
		return true;
	}
	return (piece->IsSameTeam(t));
}
bool chesscell::CaptureCell()
{
	if (piece == nullptr)
	{
		return false;
	}
	piece->CapturePiece();
	piece = nullptr;
	return true;
}
