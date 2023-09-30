#pragma once

#include "Chess/Board.h"
#include "Chess/Player.h"
#include "Core/Window.h"
#include "Renderer2D/Renderer2D.h"

#include <memory>

class Game final
{
public:
	
	Game() = default;
	~Game() = default;

	void Init(int width, int height);
	void Run();
	void Cleanup();

private:
	void InitWindow(int width, int height);

	void InitBoard();
	void FreeBoard();

	void LoadGraphics();

	void HandleInput();
	void Update(float dt);
	void Render();

	bool IsMouseButtonPressed(MouseButton button) const;

	void SelectCell(const pt2di& cellBoardPosition);
	void SwitchPlayers();

	bool OnWindowClose(const WindowCloseEvent& event);
	bool OnMouseMove(const MouseMoveEvent& event);
	bool OnMouseButtonDown(const MouseButtonDownEvent& event);
	bool OnMouseButtonUp(const MouseButtonUpEvent& event);
	bool OnKeyboardDown(const KeyboardDownEvent& event);
	bool OnKeyboardUp(const KeyboardUpEvent& event);

	std::unique_ptr<Window> m_pWindow = nullptr;
	bool m_isWindowInitialized = false;

	// mouse and keyboard states
	bool m_mouseLeftDown = false;
	bool m_mouseRightDown = false;
	bool m_mouseMiddleDown = false;
	bool m_isZPressed = false;

	Renderer2D* m_pRenderer = nullptr;

	pt2di m_mousePos{};

	bool m_isBoardInitialized = false;
	bool m_isInitialized = false;
	bool m_isPlaying = true;
	bool m_isGameOver = false;

	pt2di m_hoveredCellPos{ -1, 0 };
	pt2di m_selectedCellPos{ -1, 0 };

	Board* m_pBoard = nullptr;
	TEAM m_activeTeam = TEAM::NONE;
	Player* m_pActivePlayer = nullptr;
	Player* m_pOtherPlayer = nullptr;
	Player* m_pPlayer1 = nullptr;
	Player* m_pPlayer2 = nullptr;

	std::vector<Renderer2D::SpriteID> m_piecesSprites;
};
