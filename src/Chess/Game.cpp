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

	m_pWindow = SDL_CreateWindow(
		"Chess", // window title
		SDL_WINDOWPOS_UNDEFINED, // window x
		SDL_WINDOWPOS_UNDEFINED, // window y
		width, // width in px
		height, // height in px
		0
	);

	if (m_pRenderer != nullptr)
		delete m_pRenderer;
	
	m_pRenderer = new Renderer2D_SDL(SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	//m_pRenderer->SetWindowDim({ width, height }); // optional

	m_isSDLInitialized = true;
}

void Game::FreeSDL()
{
	if (!m_isSDLInitialized)
		return;

	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();

	m_isSDLInitialized = false;
}

void Game::InitBoard()
{
	if (m_isBoardInitialized)
		return;

	m_pBoard = new Board(8, 8);

	int width, height;
	SDL_GetWindowSize(m_pWindow, &width, &height);

	int boardWidth = m_pBoard->GetWidth();
	int boardHeight = m_pBoard->GetHeight();

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

	m_pRenderer->SetCellDim({ cellRenderWidthPx, cellRenderHeightPx });
	m_pBoard->SetCellDim(m_pRenderer->GetCellDim());

	// Player set up
	// the pieces set up should be done in the constructor
	m_pPlayer1 = new HumanPlayer(m_pBoard, TEAM::ONE, 7);
	m_pPlayer2 = new HumanPlayer(m_pBoard, TEAM::TWO, 0);

	m_pActivePlayer = m_pPlayer1;
	m_pPlayer2->AttackCells();
	m_pActivePlayer->BeginTurn();
	m_activeTeam = m_pActivePlayer->GetTeam();
	m_isBoardInitialized = true;
}

void Game::FreeBoard()
{
	if (!m_isBoardInitialized)
		return;

	delete m_pPlayer1;
	delete m_pPlayer2;

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
		Update(0.5);
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
	pt2di mouseCell = m_pBoard->WindowToBoardCoordinates(m_mousePos); // cell underneath the mouse cursor
	m_hoveredCellPos = mouseCell;

	if (m_pActivePlayer->HasEndedTurn())
		SwitchPlayers();

	if (IsMouseButtonPressed(MouseButton::LEFT)) // if the left mouse button is pressed
	{
		SelectCell(mouseCell);
	}

	m_pPlayer1->UpdatePieces(dt);
	m_pPlayer2->UpdatePieces(dt);
	m_pActivePlayer->Play({ m_selectedCellPos, 0 });
}

void Game::Render()
{
	// Draw
	m_pRenderer->Begin();
	m_pRenderer->Clear(BLACK);

	m_pBoard->DrawCells(m_pRenderer); // Draw the board
	m_pBoard->HighlightCell(m_pRenderer, m_hoveredCellPos); // highlight the cell under the mouse cursor
	m_pBoard->DrawSelectedCell(m_pRenderer, m_selectedCellPos, 7, DARK_GREEN); // highlight the selected cell
	// draw the selected piece's moves
	m_pActivePlayer->DrawSelectedPieceMoves(m_pRenderer);

	m_pPlayer1->DrawPieces(m_pRenderer);
	m_pPlayer2->DrawPieces(m_pRenderer);

	m_pPlayer1->DrawLastMove(m_pRenderer);
	m_pPlayer2->DrawLastMove(m_pRenderer);

	m_pRenderer->End();
}

void Game::SelectCell(const pt2di& cellBoardPosition)
{
	if (!m_pBoard->IsPositionValid(cellBoardPosition))
		return;

	m_selectedCellPos = cellBoardPosition;
}

void Game::SwitchPlayers()
{
	std::cout << "switching players\n";
	static int player = 0;
	// reset the guarded cells
	m_pBoard->ResetAttack();
	// we need to guard the cells before switching players
	m_pActivePlayer->AttackCells();

	// should check for check / checkmate here

	// switch players
	player = (player + 1) % 2;
	std::cout << "player " << player << "'s turn\n";
	if (player == 0)
		m_pActivePlayer = m_pPlayer1;
	else
		m_pActivePlayer = m_pPlayer2;
	m_pActivePlayer->BeginTurn();
	m_activeTeam = m_pActivePlayer->GetTeam();
}