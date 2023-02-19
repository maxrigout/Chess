#pragma once
#include <SDL2/SDL.h>

#include "Chess/Board.h"
#include "Chess/Piece.h"
#include "Chess/Player.h"
#include "Renderer2D/Renderer2D.h"

enum class MouseButton
{
	LEFT = 1,
	RIGHT = 2,
	MIDDLE = 3,
};

class Game final
{
public:
	
	Game();
	~Game();

	void Init(int width, int height);
	void Run();
	void Cleanup();

private:
	void InitSDL(int width, int height);
	void FreeSDL();

	void InitBoard();
	void FreeBoard();

	void LoadGraphics();

	void HandleInput();
	void Update(float dt);
	void Render();

	bool IsMouseButtonPressed(MouseButton button);

	void SelectCell(const pt2di& cellBoardPosition);
	void SwitchPlayers();

	SDL_Window* m_pWindow;
	pt2di m_mousePos;
	unsigned int m_mouseButtonState;

	Renderer2D* m_pRenderer = nullptr;

	bool m_isInitialized = false;
	bool m_isSDLInitialized = false;
	bool m_isBoardInitialized = false;
	bool m_isPlaying;
	bool m_isGameOver = false;

	pt2di m_hoveredCellPos{-1, 0};
	pt2di m_selectedCellPos{-1, 0};

	Board* m_pBoard;
	TEAM m_activeTeam = TEAM::NONE;
	Player* m_pActivePlayer;
	Player* m_pPlayer1;
	Player* m_pPlayer2;

	std::vector<Renderer2D::SpriteID> m_piecesSprites;
};