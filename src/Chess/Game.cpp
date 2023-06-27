#include "Game.h"
#include "Core/Factories/WindowFactory.h"
#include "Core/Config/SystemConfiguration.h"
#include "Chess/Factory/PlayerFactory.h"

#define DEBUG
#include "Assets.h"
#include "Core/Logger.h"

#define MAX_AI_THINKING_TIMEOUT_SECOND 10

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::Init(int width, int height)
{
	InitWindow(width, height);
	InitBoard();
	LoadGraphics();
	m_isInitialized = m_isWindowInitialized && m_isBoardInitialized;
}

void Game::InitWindow(int width, int height)
{
	WindowCreationInfo info{};
	info.title = "Chess";
	info.width = width;
	info.height = height;
	info.flags.rendererBackend = RendererConfiguration::GetRendererBackendType();
	m_pWindow = WindowFactory::CreateWindow();
	m_pWindow->Create(info);
	m_pRenderer = m_pWindow->CreateRenderer();

	// Setting the callbacks
	m_pWindow->OnWindowClose(std::bind(&Game::OnWindowClose, this, std::placeholders::_1));
	m_pWindow->OnMouseMove(std::bind(&Game::OnMouseMove, this, std::placeholders::_1));
	m_pWindow->OnMouseButtonDown(std::bind(&Game::OnMouseButtonDown, this, std::placeholders::_1));
	m_pWindow->OnMouseButtonUp(std::bind(&Game::OnMouseButtonUp, this, std::placeholders::_1));
	m_pWindow->OnKeyboardDown(std::bind(&Game::OnKeyboardDown, this, std::placeholders::_1));
	m_pWindow->OnKeyboardUp(std::bind(&Game::OnKeyboardUp, this, std::placeholders::_1));

	m_isWindowInitialized = true;
}

void Game::InitBoard()
{
	if (m_isBoardInitialized)
		return;

	m_pBoard = new Board(8, 8);
	m_pBoard->SetScreenDim(m_pRenderer->GetViewPortDim());
	vec2di baseMargin = { 95, 95 };
	vec2di baseCellSize = { 137, 137 };
	vec2di baseBoardTextureSize = { 1280, 1280 };
	int width, height;
	width = m_pWindow->GetWidth();
	height = m_pWindow->GetHeight();

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
	m_pBoard->CreatePieces();

	// Player set up
	// the pieces set up should be done in the constructor
	m_pPlayer1 = PlayerFactory::CreatePlayer1(m_pBoard);
	m_pPlayer2 = PlayerFactory::CreatePlayer2(m_pBoard);

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

	delete m_pPlayer2;
	delete m_pPlayer1;
	delete m_pBoard;

	m_isBoardInitialized = false;
}

void Game::LoadGraphics()
{
	std::vector<SpriteDescriptor> sprites;
	sprites.push_back({ { 201, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "P1" });
	sprites.push_back({ { 415, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "H1" });
	sprites.push_back({ { 628, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "B1" });
	sprites.push_back({ { 842, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "R1" });
	sprites.push_back({ { 1056, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "Q1" });
	sprites.push_back({ { 1270, 989 }, { 191, 191 }, SpriteOffsetType::BottomRight, "K1" });

	sprites.push_back({ { 201, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "P2" });
	sprites.push_back({ { 415, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "H2" });
	sprites.push_back({ { 628, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "B2" });
	sprites.push_back({ { 842, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "R2" });
	sprites.push_back({ { 1056, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "Q2" });
	sprites.push_back({ { 1270, 1196 }, { 191, 191 }, SpriteOffsetType::BottomRight, "K2" });
	m_piecesSprites = m_pRenderer->LoadSpriteSheet(PIECES_TEXTURE_PATH, sprites);

	m_pRenderer->LoadTexture(BOARD_TEXTURE_PATH, "board");

	m_pRenderer->LoadTexture(AVAILABLE_CELLS_PATH, "availableCell");
	m_pRenderer->LoadTexture(HOVERED_CELLS_PATH, "hoveredCell");
	m_pRenderer->LoadTexture(SELECTED_CELLS_PATH, "selectedCell");
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
	m_pWindow->FreeRenderer();
	// delete m_pRenderer;
	delete m_pWindow;
}

void Game::HandleInput()
{
	m_pWindow->PollEvents();
}

bool Game::IsMouseButtonPressed(MouseButton button)
{
	switch (button)
	{
	case MouseButton::Left: return m_mouseLeftDown;
	case MouseButton::Right: return m_mouseRightDown;
	case MouseButton::Middle: return m_mouseMiddleDown;
	default: break;
	}
	return false;
}

void Game::Update(float dt)
{
	pt2di mouseCell = m_pBoard->WindowToBoardCoordinates(m_mousePos); // cell underneath the mouse cursor
	m_hoveredCellPos = mouseCell;

	if (m_pActivePlayer->HasEndedTurn())
		SwitchPlayers();

	if (IsMouseButtonPressed(MouseButton::Left)) // if the left mouse button is pressed
	{
		SelectCell(mouseCell);
	}

	m_pPlayer1->UpdatePieces(dt);
	m_pPlayer2->UpdatePieces(dt);
	if (!m_isGameOver)
		m_pActivePlayer->Play({ m_selectedCellPos, MAX_AI_THINKING_TIMEOUT_SECOND, dt, !m_isPlaying });
}

void Game::Render()
{
	// Draw
	m_pRenderer->Begin();
	m_pRenderer->Clear(BLUE);

	m_pBoard->DrawBoard(m_pRenderer); // Draw the board
	m_pBoard->HighlightCell(m_pRenderer, m_hoveredCellPos); // highlight the cell under the mouse cursor
	m_pBoard->DrawSelectedCell(m_pRenderer, m_selectedCellPos, 7, DARK_GREEN); // highlight the selected cell
	// draw the selected piece's moves
	m_pActivePlayer->DrawSelectedPieceMoves(m_pRenderer);

	// m_pBoard->DrawCellLabels(m_pRenderer, DARK_GREY);

	m_pPlayer1->DrawPieces(m_pRenderer);
	m_pPlayer2->DrawPieces(m_pRenderer);

	m_pPlayer1->DrawLastMove(m_pRenderer);
	m_pPlayer2->DrawLastMove(m_pRenderer);

	m_pBoard->DrawHourglass(m_pRenderer);

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
	std::string msg = "player " + std::to_string(player + 1) + "'s turn";
	LOG_INFO(msg);
	msg = "Chess - " + msg;
	m_pWindow->SetTitle(msg);
}

bool Game::OnWindowClose(const WindowCloseEvent& event) 
{
	m_isPlaying = false;
	return true;
}

bool Game::OnMouseMove(const MouseMoveEvent& event) 
{
	m_mousePos = { event.x, event.y };
	return true;
}

bool Game::OnMouseButtonDown(const MouseButtonDownEvent& event) 
{
	switch (event.button)
	{
	case MouseButton::Left: m_mouseLeftDown = true; return true;
	case MouseButton::Right: m_mouseRightDown = true; return true;
	case MouseButton::Middle: m_mouseMiddleDown = true; return true;
	default: break;
	}
	return false;
}

bool Game::OnMouseButtonUp(const MouseButtonUpEvent& event) 
{
	switch (event.button)
	{
	case MouseButton::Left: m_mouseLeftDown = false; return true;
	case MouseButton::Right: m_mouseRightDown = false; return true;
	case MouseButton::Middle: m_mouseMiddleDown = false; return true;
	default: break;
	}
	return false;
}

bool Game::OnKeyboardDown(const KeyboardDownEvent& event) 
{
	if (event.key == Key::Z && !m_isZPressed)
	{
		m_isZPressed = true;
		if (m_pBoard->UndoMove())
			SwitchPlayers();
		return true;
	}
	return false;
}

bool Game::OnKeyboardUp(const KeyboardUpEvent& event) 
{
	if (event.key == Key::Z)
	{
		m_isZPressed = false;
		return true;
	}
	return false;
}
