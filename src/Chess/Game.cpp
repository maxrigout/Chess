#include "Game.h"
#include "Core/Factories/WindowFactory.h"
#include "Chess/Factory/PlayerFactory.h"
#include "Core/Config/SystemConfiguration.h"
#include "Renderer2D/Config/RendererConfiguration.h"

#define DEBUG
#include "Assets.h"
#include "Core/Logger.h"

#include <chrono>

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
	m_pWindow->OnWindowClose([this](const WindowCloseEvent& event) {
        return this->OnWindowClose(event);
    });
	m_pWindow->OnMouseMove([this](const MouseMoveEvent& event) {
        return this->OnMouseMove(event);
    });
	m_pWindow->OnMouseButtonDown([this](const MouseButtonDownEvent& event) {
        return this->OnMouseButtonDown(event);
    });
	m_pWindow->OnMouseButtonUp([this](const MouseButtonUpEvent& event) {
        return this-> OnMouseButtonUp(event);
    });
	m_pWindow->OnKeyboardDown([this](const KeyboardDownEvent& event) {
        return this->OnKeyboardDown(event);
    });
	m_pWindow->OnKeyboardUp([this](const KeyboardUpEvent& event) {
        return this->OnKeyboardUp(event);
    });

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
    // TODO: file
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
	// auto startTime = std::chrono::high_resolution_clock::now();
	auto frameStart = std::chrono::high_resolution_clock::now();
	auto frameEnd = std::chrono::high_resolution_clock::now();
	while(m_isPlaying)
	{
		// float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(frameEnd - startTime).count();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(frameEnd - frameStart).count();
		frameStart = std::chrono::high_resolution_clock::now();
		HandleInput();
		Update(dt);
		Render();
		frameEnd = std::chrono::high_resolution_clock::now();
	}
}

void Game::Cleanup()
{
	FreeBoard();
	m_pWindow->FreeRenderer();
	m_pWindow = nullptr;
}

void Game::HandleInput()
{
	m_pWindow->PollEvents();
}

bool Game::IsMouseButtonPressed(MouseButton button) const
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
		m_pActivePlayer->Play({ m_selectedCellPos, dt, !m_isPlaying });
}

void Game::Render()
{
	// Draw
	m_pRenderer->Begin();
	m_pRenderer->Clear(RED);

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
	{
		m_pActivePlayer = m_pPlayer1;
		m_pOtherPlayer = m_pPlayer2;
	}
	else
	{
		m_pActivePlayer = m_pPlayer2;
		m_pOtherPlayer = m_pPlayer1;
	}
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
	}
	return false;
}

bool Game::OnKeyboardDown(const KeyboardDownEvent& event) 
{
	if (event.key == Key::Z && !m_isZPressed)
	{
		LOG_TRACE("Z key down");
		m_isZPressed = true;
		if (m_pOtherPlayer->AllowsUndos())
		{
			// undo 2 moves
			if (m_pBoard->UndoMove())
			{
				if (m_pBoard->UndoMove())
				{
					LOG_DEBUG("Undo Successful");
					return true;
				} else
				{
					LOG_DEBUG("Was only able to undo 1 move... Switching Players");
					SwitchPlayers();
				}
			}
			else
			{
				LOG_DEBUG("cannot undo");
			}
		}
		else
		{
			LOG_DEBUG("other player doesn't allow undos!");
		}
	}
	return false;
}

bool Game::OnKeyboardUp(const KeyboardUpEvent& event) 
{
	LOG_TRACE("OnKeyboardUp");
	if (event.key == Key::Z)
	{
		LOG_TRACE("Z key up");
		m_isZPressed = false;
		return true;
	}
	return false;
}
