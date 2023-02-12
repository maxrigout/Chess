#include "Game.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Chess/Players/HumanPlayer.h"

#include "Renderer2D/SDL/Renderer2D_SDL.h"

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
	m_isInitialized = m_isSDLInitialized && m_isBoardInitialized;
}

void Game::InitSDL(int width, int height)
{
	if (m_isSDLInitialized)
		return;
	
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

	if (m_renderer != nullptr)
		delete m_renderer;
	
	m_renderer = new Renderer2D_SDL(SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	//m_renderer->SetWindowDim({ width, height }); // optional

	m_isSDLInitialized = true;
}

void Game::FreeSDL()
{
	if (!m_isSDLInitialized)
		return;

	SDL_DestroyWindow(m_window);
	SDL_Quit();

	m_isSDLInitialized = false;
}

void Game::InitBoard()
{
	if (m_isBoardInitialized)
		return;

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

	m_renderer->SetCellDim({ cellRenderWidthPx, cellRenderHeightPx });

	// Player set up
	// the pieces set up should be done in the constructor
	m_player1 = new HumanPlayer(&m_board, TEAM::ONE, 7);
	m_player2 = new HumanPlayer(&m_board, TEAM::TWO, 0);

	m_activePlayer = m_player1;
	activeTeam = m_player1->GetTeam();
	m_isBoardInitialized = true;
}

void Game::FreeBoard()
{
	if (!m_isBoardInitialized)
		return;

	delete m_player1;
	delete m_player2;

	m_isBoardInitialized = false;
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
	pt2di mouseCell = m_board.WindowToBoardCoordinates(m_mousePos, m_renderer->GetCellDim()); // cell underneath the mouse cursor
	Cell* selectedCell = m_board.GetCell(m_selectedCellPos);
	m_hoveredCellPos = mouseCell;

	if (m_activePlayer->HasEndedTurn())
		SwitchPlayers();

	if (IsMouseButtonPressed(MouseButton::LEFT)) // if the left mouse button is pressed
	{
		SelectCell(mouseCell);

		if (selectedCell == nullptr)
			return;

		if (m_selectedPiece == nullptr)
		{
			m_selectedPiece = m_activePlayer->GetPieceAtPosition(m_selectedCellPos);
			return;
		}

		if (m_selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move is valid
		{
			m_selectedPiece->Move(mouseCell); // move the piece
			SwitchPlayers(); // end the current player's turn and allow the other player to play
		}
		else if (!m_selectedPiece->IsMoveValid(mouseCell)) // if we selected a piece and the move isn't valid
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
	m_renderer->Begin();
	m_renderer->Clear(BLACK);

	m_board.DrawCells(m_renderer); // Draw the board
	m_board.HighlightCell(m_renderer, m_hoveredCellPos); // highlight the cell under the mouse cursor
	m_board.DrawSelectedCell(m_renderer, m_selectedCellPos, 7); // highlight the selected cell
	
	if (m_selectedPiece != nullptr)
	{
		m_selectedPiece->DrawMoves(m_renderer); // draw the selected piece's moves
	}

	m_player1->DrawPieces(m_renderer);
	m_player2->DrawPieces(m_renderer);

	m_renderer->End();
}

void Game::SelectCell(pt2di cellBoardPosition)
{
	if (!m_board.IsPositionValid(cellBoardPosition))
		return;

	m_selectedCellPos = cellBoardPosition;
}

void Game::SwitchPlayers()
{
	m_selectedPiece = nullptr; // de-select the piece

	static int player = 0;
	// reset the guarded cells
	m_board.ResetGuard();
	// we need to guard the cells before switching players
	m_activePlayer->GuardCells();

	// should check for check / checkmate here

	// switch players
	player = (player + 1) % 2;
	if (player == 0)
		m_activePlayer = m_player1;
	else
		m_activePlayer = m_player2;

	m_activePlayer->BeginTurn();
	activeTeam = m_activePlayer->GetTeam();
}