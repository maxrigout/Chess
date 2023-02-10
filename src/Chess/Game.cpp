#include "Game.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Tower.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include <iostream>


Game::Game()
	: m_board(8, 8)
{
	
}

Game::~Game()
{

}

void Game::Init(int width, int height)
{
	InitSDL(width, height);
	InitBoard();
	m_isInitialized = true;
}

void Game::InitSDL(int width, int height)
{
	SDL_Init(SDL_INIT_VIDEO);

	// SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

	m_window = SDL_CreateWindow(
		"Chess", // window title
		SDL_WINDOWPOS_UNDEFINED, // window x
		SDL_WINDOWPOS_UNDEFINED, // window y
		width, // width in px
		height, // height in px
		0
	);

	m_sdlRenderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_renderer.renderer = m_sdlRenderer;
	m_renderer.windowWidth = width;
	m_renderer.windowHeight = height;
}

void Game::FreeSDL()
{
	SDL_DestroyRenderer(m_sdlRenderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Game::InitBoard()
{
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);

	int boardWidth = m_board.GetWidth();
	int boardHeight = m_board.GetHeight();

	bool square_cells = true; // if we want the cells to be square

	int cellRenderWidthPx, cellRenderHeightPx;
	if (square_cells)
	{
		int val = (width / boardWidth > height / boardHeight) ? height / boardHeight : width / boardWidth;
		cellRenderWidthPx = val;
		cellRenderHeightPx = val;
	}
	else
	{
		cellRenderWidthPx = width / boardWidth;
		cellRenderHeightPx = height / boardHeight;
	}

	m_renderer.cellWidth = cellRenderWidthPx;
	m_renderer.cellHeight = cellRenderHeightPx;

	TEAM team = TEAM::ONE;
	Piece* t1, * t2, * b1, * b2, * h1, * h2, * q, * k;
	t1	=	new	Tower   (&m_board, team, { 0, 7 });
	h1	=	new	Knight	(&m_board, team, { 1, 7 });
	b1	=	new	Bishop	(&m_board, team, { 2, 7 });
	q	=	new	Queen	(&m_board, team, { 3, 7 });
	k	=	new	King	(&m_board, team, { 4, 7 });
	b2	=	new	Bishop	(&m_board, team, { 5, 7 });
	h2	=	new	Knight	(&m_board, team, { 6, 7 });
	t2	=	new	Tower	(&m_board, team, { 7, 7 });
	player1Pieces = { t1, h1, b1, q, k, b2, h2, t2 };


	for (int i = 0; i < 8; i++)
	{
		Piece* p = new Pawn(&m_board, team, { i, 6 });
		player1Pieces.push_back(p);
	}

	for (auto piece : player1Pieces)
	{
		int x = piece->Pos().x;
		int y = piece->Pos().y;
		m_board.GetCell({x, y})->PlacePiece(piece);
	}

	team = TEAM::TWO;
	t1	= new	Tower	(&m_board, team, { 0, 0 });
	h1	= new	Knight	(&m_board, team, { 1, 0 });
	b1	= new	Bishop	(&m_board, team, { 2, 0 });
	q	= new	Queen	(&m_board, team, { 3, 0 });
	k	= new	King	(&m_board, team, { 4, 0 });
	b2	= new	Bishop	(&m_board, team, { 5, 0 });
	h2	= new	Knight	(&m_board, team, { 6, 0 });
	t2	= new	Tower	(&m_board, team, { 7, 0 });
	player2Pieces = { t1, h1, b1, q, k, b2, h2, t2 };

	for (int i = 0; i < 8; i++)
	{
		Piece* p = new Pawn(&m_board, team, { i, 1 });
		player2Pieces.push_back(p);
	}

	for (auto piece : player2Pieces)
	{
		int x = piece->Pos().x;
		int y = piece->Pos().y;
		m_board.GetCell({x, y})->PlacePiece(piece);
	}

	// SelectCell({ 0, 0 });
	activePlayer = &player1Pieces;
	activeTeam = activePlayer->at(0)->Team();
}

void Game::FreeBoard()
{
	for (auto p : player1Pieces)
		delete p;
	
	for (auto p : player2Pieces)
		delete p;
}

void Game::Run()
{
	if (!m_isInitialized)
		return;
	m_isPlaying = true;
	while(m_isPlaying)
	{
		HandleInput();
		Update(0.16);
		Render();
	}
}

void Game::Cleanup()
{
	FreeBoard();
	FreeSDL();
}

void Game::HandleInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				m_isPlaying = false;
				break;

			case SDL_MOUSEMOTION:
				m_mousePos.x = event.motion.x;
				m_mousePos.y = event.motion.y;
				// std::cout << "SDL_MOUSEMOTION: (" << m_mousePos.x << ", " << m_mousePos.y << ")\n";
				break;

			case SDL_MOUSEBUTTONUP:
				m_mouseButtonState = 0;
				break;

			case SDL_MOUSEBUTTONDOWN:
				m_mouseButtonState = event.button.button;
				break;

			case SDL_MOUSEWHEEL:
				std::cout << "SDL_MOUSEWHEEL: " << event.wheel.y << "\n";
				break;

			case SDL_KEYDOWN:
				std::cout << "SDL_KEYDOWN: " << event.key.keysym.scancode << "\n";
				break;

			case SDL_KEYUP:
				std::cout << "SDL_KEYUP: " << event.key.keysym.scancode << "\n";
				break;
		}
	}
}

bool Game::IsMouseButtonPressed(MouseButton button)
{
	uint32_t mask = 0;
	switch (button)
	{
	case MouseButton::LEFT:
		mask |= SDL_BUTTON_LMASK;
		break;
	case MouseButton::RIGHT:
		mask |= SDL_BUTTON_RMASK;
		break;
	case MouseButton::MIDDLE:
		mask |= SDL_BUTTON_MMASK;
		break;
	
	default:
		break;
	}
	return m_mouseButtonState & mask;
}

void Game::Update(float dt)
{
	point2d<int> mouseCell = m_board.WindowToBoardCoordinates(m_mousePos, {m_renderer.cellWidth, m_renderer.cellHeight}); // cell underneath the mouse cursor
	// Piece* selectedPiece = m_board.GetSelectedPiece();
	Cell* selectedCell = m_board.GetCell(m_selectedCellPos);
	m_hoveredCellPos = mouseCell;

	if (IsMouseButtonPressed(MouseButton::LEFT)) // if the left mouse button is pressed
	{
		SelectCell(mouseCell);
		if (m_selectedPiece == nullptr && selectedCell->HasPiece()) { // if we don't have any piece selected
			if (selectedCell->GetPiece()->IsSameTeam(activeTeam) && !selectedCell->GetPiece()->IsCaptured())
			{
				m_selectedPiece = selectedCell->GetPiece();
			}
		}
		else if (m_selectedPiece != nullptr && m_selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move is valid
		{
			m_selectedPiece->Move(mouseCell); // move the piece
			m_selectedPiece = nullptr; // de-select the piece
			SwitchPlayers(); // end the current player's turn and allow the other player to play
		}
		else if (m_selectedPiece != nullptr && !m_selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move isn't valid
		{
			m_selectedPiece = nullptr;
			if (selectedCell->HasPiece()) { // check to see if we can select another piece
				if (selectedCell->GetPiece()->IsSameTeam(activeTeam) && !selectedCell->GetPiece()->IsCaptured())
				{
					m_selectedPiece = selectedCell->GetPiece();
				}
			}
		}
	}
}

void Game::Render()
{
	// Draw
	SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 0);
	SDL_RenderClear(m_sdlRenderer); // Clear the screen

	m_board.DrawCells(&m_renderer); // Draw the board
	m_board.HighlightCell(&m_renderer, m_hoveredCellPos); // highlight the cell under the mouse cursor
	m_board.DrawSelectedCell(&m_renderer, m_selectedCellPos, 7); // highlight the selected cell
	if (m_selectedPiece != nullptr)
	{
		m_selectedPiece->DrawMoves(&m_renderer); // draw the selected piece's moves
	}

	// draw the player 1's pieces
	for (auto p : player1Pieces)
	{
		p->DrawYourself(&m_renderer);
	}

	// draw the player 2's pieces
	for (auto p : player2Pieces)
	{
		p->DrawYourself(&m_renderer);
	}

	SDL_RenderPresent(m_sdlRenderer);
}

void Game::SelectCell(point2d<int> cellBoardPosition)
{
	m_selectedCellPos = cellBoardPosition;
}

void Game::SwitchPlayers()
{
	static int player = 0;
	// reset the guarded cells
	for (int j = 0; j < m_board.GetHeight(); ++j) 
	{
		for (int i = 0; i < m_board.GetWidth(); ++i)
		{
			m_board.GetCell({i, j})->ResetGuarded();
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
		activePlayer = &player1Pieces;
	}
	else
	{
		activePlayer = &player2Pieces;
	}
	activeTeam = activePlayer->at(0)->Team();
}