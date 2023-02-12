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

	void HandleInput();
	void Update(float dt);
	void Render();

	bool IsMouseButtonPressed(MouseButton button);

	void SelectCell(point2d<int> cellBoardPosition);
	void SwitchPlayers();

	SDL_Window* m_window;
	point2d<int> m_mousePos;
	unsigned int m_mouseButtonState;

	Renderer2D* m_renderer = nullptr;

	bool m_isInitialized = false;
	bool m_isSDLInitialized = false;
	bool m_isBoardInitialized = false;
	bool m_isPlaying;

	point2d<int> m_hoveredCellPos{-1, 0};
	point2d<int> m_selectedCellPos{-1, 0};

	Board m_board;
	// Piece* m_selectedPiece = nullptr;
	// Cell* m_selectedCell;

	// std::vector<Piece*>* activePlayerPieces;
	TEAM activeTeam = TEAM::NONE;
	Player* m_activePlayer;
	Player* m_player1;
	Player* m_player2;
	// std::vector<Piece*> player1Pieces;
	// std::vector<Piece*> player2Pieces;
};