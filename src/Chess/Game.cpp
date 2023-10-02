#include "Game.h"
#include "Core/Factories/WindowFactory.h"
#include "Chess/Factory/PlayerFactory.h"
#include "Core/Config/SystemConfiguration.h"
#include "Renderer2D/Config/RendererConfiguration.h"
#include "Assets.h"
#include "Core/Logger.h"
#include <nlohmann/json.hpp>
#include <chrono>

#define DEBUG

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
	auto loadSpriteFromFile = [](const char* path) {
		std::ifstream f(path);
		if (!f.is_open())
		{
			LOG_ERROR(std::string("cannot open file: ") + path);
			return SpriteDescriptor{};
		}
		nlohmann::json data = nlohmann::json::parse(f);
		SpriteDescriptor desc;
		desc.texturePath = data["texture_path"];
		desc.offset.x = data["offset"]["x"];
		desc.offset.y = data["offset"]["y"];
		desc.size.x = data["size"]["x"];
		desc.size.y = data["size"]["y"];
		desc.tag = data["tag"];

		// offsetType
		std::string offsetType = data["offsetType"];
		if (offsetType == "Top Left")
		{
			desc.offsetType = SpriteOffsetType::TopLeft;
		}
		else if (offsetType == "Top Right")
		{
			desc.offsetType = SpriteOffsetType::TopRight;
		}
		else if (offsetType == "Bottom Left")
		{
			desc.offsetType = SpriteOffsetType::BottomLeft;
		}
		else if (offsetType == "Bottom Right")
		{
			desc.offsetType = SpriteOffsetType::BottomRight;
		}
		else
		{
			LOG_ERROR("got unknown offset type when reading sprite file: " + offsetType);
		}

		return desc;
	};

	auto wp = loadSpriteFromFile("resources/sprites/whites/pawn.json");
	auto wh = loadSpriteFromFile("resources/sprites/whites/knight.json");
	auto wb = loadSpriteFromFile("resources/sprites/whites/bishop.json");
	auto wr = loadSpriteFromFile("resources/sprites/whites/rook.json");
	auto wq = loadSpriteFromFile("resources/sprites/whites/queen.json");
	auto wk = loadSpriteFromFile("resources/sprites/whites/king.json");

	auto bp = loadSpriteFromFile("resources/sprites/blacks/pawn.json");
	auto bh = loadSpriteFromFile("resources/sprites/blacks/knight.json");
	auto bb = loadSpriteFromFile("resources/sprites/blacks/bishop.json");
	auto br = loadSpriteFromFile("resources/sprites/blacks/rook.json");
	auto bq = loadSpriteFromFile("resources/sprites/blacks/queen.json");
	auto bk = loadSpriteFromFile("resources/sprites/blacks/king.json");

	auto boardDesc = loadSpriteFromFile("resources/sprites/board.json");
	auto availableCellDesc = loadSpriteFromFile("resources/sprites/available_cell.json");
	auto hoveredCellDesc = loadSpriteFromFile("resources/sprites/hovered_cell.json");
	auto selectedCellDesc = loadSpriteFromFile("resources/sprites/selected_cell.json");

	m_pRenderer->LoadSprite(wp);
	m_pRenderer->LoadSprite(wh);
	m_pRenderer->LoadSprite(wb);
	m_pRenderer->LoadSprite(wr);
	m_pRenderer->LoadSprite(wq);
	m_pRenderer->LoadSprite(wk);

	m_pRenderer->LoadSprite(bp);
	m_pRenderer->LoadSprite(bh);
	m_pRenderer->LoadSprite(bb);
	m_pRenderer->LoadSprite(br);
	m_pRenderer->LoadSprite(bq);
	m_pRenderer->LoadSprite(bk);

	m_pRenderer->LoadSprite(boardDesc);
	m_pRenderer->LoadSprite(availableCellDesc);
	m_pRenderer->LoadSprite(hoveredCellDesc);
	m_pRenderer->LoadSprite(selectedCellDesc);
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
	case MouseButton::X1:
	case MouseButton::X2:
	case MouseButton::N_MOUSE_BUTTONS:
		break;
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
	case MouseButton::X1:
	case MouseButton::X2:
	case MouseButton::N_MOUSE_BUTTONS:
		break;
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
	case MouseButton::X1:
	case MouseButton::X2:
	case MouseButton::N_MOUSE_BUTTONS:
		break;
	}
	return false;
}

bool Game::OnKeyboardDown(const KeyboardDownEvent& event) 
{
	if (event.key == Key::Z)
	{
		if (m_isZPressed)
			// z is already pressed!
			return false;
		if (!m_pOtherPlayer->AllowsUndos())
		{
			LOG_DEBUG("other player doesn't allow undos!");
			return false;
		}
		// undo 2 moves
		if (m_pBoard->UndoMove())
		{
			if (m_pBoard->UndoMove())
			{
				LOG_DEBUG("Undo Successful");
				return true;
			}
			else
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
