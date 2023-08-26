#pragma once

#include "Chess/Board.h"
#include "Chess/Player.h"
#include "Core/Window.h"
#include "Renderer2D/Renderer2D.h"

class Game final
{
public:
	
	Game();
	~Game();

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

	bool IsMouseButtonPressed(MouseButton button);

	void SelectCell(const pt2di& cellBoardPosition);
	void SwitchPlayers();

	bool OnWindowClose(const WindowCloseEvent& event);
	bool OnMouseMove(const MouseMoveEvent& event);
	bool OnMouseButtonDown(const MouseButtonDownEvent& event);
	bool OnMouseButtonUp(const MouseButtonUpEvent& event);
	bool OnKeyboardDown(const KeyboardDownEvent& event);
	bool OnKeyboardUp(const KeyboardUpEvent& event);

	Window* m_pWindow;
	bool m_mouseLeftDown = false;
	bool m_mouseRightDown = false;
	bool m_mouseMiddleDown = false;
	bool m_isZPressed = false;
	bool m_isWindowInitialized = false;

	Renderer2D* m_pRenderer = nullptr;

	pt2di m_mousePos;

	bool m_isBoardInitialized = false;
	bool m_isInitialized = false;
	bool m_isPlaying = true;
	bool m_isGameOver = false;

	pt2di m_hoveredCellPos{ -1, 0 };
	pt2di m_selectedCellPos{ -1, 0 };

	Board* m_pBoard;
	TEAM m_activeTeam = TEAM::NONE;
	Player* m_pActivePlayer;
	Player* m_pPlayer1;
	Player* m_pPlayer2;

	std::vector<Renderer2D::SpriteID> m_piecesSprites;
};
