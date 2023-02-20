#include "Game.h"
#include "Chess/Pieces/Pawn.h"
#include "Chess/Pieces/Rook.h"
#include "Chess/Pieces/Knight.h"
#include "Chess/Pieces/Bishop.h"
#include "Chess/Pieces/Queen.h"
#include "Chess/Pieces/King.h"

#include "Chess/Players/HumanPlayer.h"

#include "Renderer2D/SDL/Renderer2D_SDL.h"

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
	LoadGraphics();
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
	m_pBoard->SetScreenDim(m_pRenderer->GetWindowDim());
	vec2di baseMargin = { 95, 95 };
	vec2di baseCellSize = { 137, 137 };
	vec2di baseBoardTextureSize = { 1280, 1280 };
	int width, height;
	SDL_GetWindowSize(m_pWindow, &width, &height);

	int boardWidth = m_pBoard->GetWidth();
	int boardHeight = m_pBoard->GetHeight();

	bool square_cells = true; // if we want the cells to be square

	vec2di cellRenderPx;
	if (square_cells)
	{
		int val = (width / boardWidth > height / boardHeight) ? height / boardHeight : width / boardWidth;
		cellRenderPx = { val, val };

		int winSz = std::min(width, height);
		int texSz = std::min(baseBoardTextureSize.w, baseBoardTextureSize.h);
		float scale = (float)winSz / (float)texSz;
		baseCellSize.w = (int)((float)baseCellSize.w * scale);
		baseCellSize.h = (int)((float)baseCellSize.h * scale);

		baseMargin.w = (int)((float)baseMargin.w * scale);
		baseMargin.h = (int)((float)baseMargin.h * scale);
		cellRenderPx = baseCellSize;
	}
	else
	{
		cellRenderPx = { width / boardWidth, height / boardHeight };
	}

	m_pBoard->SetMargin(baseMargin);

	m_pRenderer->SetCellDim(cellRenderPx);
	m_pBoard->SetCellDim(m_pRenderer->GetCellDim());

	// Player set up
	// the pieces set up should be done in the constructor
	m_pPlayer1 = new HumanPlayer(m_pBoard, TEAM::ONE);
	m_pPlayer2 = new HumanPlayer(m_pBoard, TEAM::TWO);
	// m_pPlayer2 = new AIPlayer(m_pBoard, TEAM::TWO);

	// ((AIPlayer*)m_pPlayer2)->SetOpponentPlayer(m_pPlayer1);

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

void Game::LoadGraphics()
{
	std::vector<SpriteDescriptor> sprites;
	sprites.push_back({ { 201, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "P2" });
	sprites.push_back({ { 415, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "H2" });
	sprites.push_back({ { 628, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "B2" });
	sprites.push_back({ { 842, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "R2" });
	sprites.push_back({ { 1056, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "Q2" });
	sprites.push_back({ { 1270, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "K2" });

	sprites.push_back({ { 201, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "P1" });
	sprites.push_back({ { 415, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "H1" });
	sprites.push_back({ { 628, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "B1" });
	sprites.push_back({ { 842, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "R1" });
	sprites.push_back({ { 1056, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "Q1" });
	sprites.push_back({ { 1270, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "K1" });
	m_piecesSprites = m_pRenderer->LoadSpriteSheet("resources/graphics/pieces3.png", sprites);

	m_pRenderer->LoadTexture("resources/graphics/board2.png", "board");
}

void Game::Run()
{
	if (!m_isInitialized)
		return;

	m_isPlaying = true;
	while(m_isPlaying)
	{
		// TODO frame limit cap
		HandleInput();
		Update(0.0166);
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
	static bool zDown = false;
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
				// std::cout << "SDL_MOUSEWHEEL: " << event.wheel.y << "\n";
				break;

			case SDL_KEYDOWN:
				// std::cout << "SDL_KEYDOWN: " << event.key.keysym.scancode << "\n";
				if (event.key.keysym.sym == SDLK_z && !zDown)
				{
					zDown = true;
					m_pBoard->UndoMove();
					SwitchPlayers();
				}
				break;

			case SDL_KEYUP:
				// std::cout << "SDL_KEYUP: " << event.key.keysym.scancode << "\n";
				if (event.key.keysym.sym == SDLK_z)
					zDown = false;
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
	if (!m_isGameOver)
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

	// m_pBoard->DrawCellLabels(m_pRenderer, DARK_GREY);

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
	LOG_INFO("switching players");
	static int player = 0;
	// reset the guarded cells
	m_pBoard->ResetAttack();
	// we need to guard the cells before switching players
	m_pActivePlayer->AttackCells();

	// should check for check / checkmate here

	// switch players
	player = (player + 1) % 2;
	if (player == 0)
		m_pActivePlayer = m_pPlayer1;
	else
		m_pActivePlayer = m_pPlayer2;
	m_pActivePlayer->BeginTurn();
	m_activeTeam = m_pActivePlayer->GetTeam();
	if (m_pActivePlayer->IsCheckMate())
	{
		LOG_INFO("game: checkmate\n");
		m_isGameOver = true;
		return;
	}
	LOG_INFO("player " + std::to_string(player + 1) + "'s turn");
}