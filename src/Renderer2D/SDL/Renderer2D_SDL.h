#pragma once

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "Renderer2D/Renderer2D.h"

class Renderer2D_SDL : public Renderer2D
{
public:
	Renderer2D_SDL(SDL_Renderer* renderer);
	virtual ~Renderer2D_SDL();

	virtual void Begin();
	virtual void End();
	virtual void Clear(const Color& color) const;
	virtual void DrawDisk(point2d<int> center, int radius, const Color& color) const;
	virtual void DrawCircle(point2d<int> center, int radius, const Color& color) const;
	virtual void FillCircle(point2d<int> center, int radius, const Color& color) const;
	virtual void DrawRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const;
	virtual void FillRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const;
	virtual void DrawLine(point2d<int> start, point2d<int> end, const Color& color) const;
	virtual void DrawText(point2d<int> position, const std::string& text, const Color& color) const;

	virtual vector2d<int> GetWindowDim() const;
	virtual vector2d<int> GetCellDim() const;
	virtual vector2d<int> GetViewPortDim() const;
	virtual void SetWindowDim(vector2d<int> dim);
	virtual void SetCellDim(vector2d<int> dim);
	virtual void SetViewPortDim(vector2d<int> dim);

private:
	void SetRenderDrawColor(const Color& c) const { SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a); }

	SDL_Renderer* m_renderer;
	vector2d<int> m_windowDim;
	vector2d<int> m_cellDim;
	vector2d<int> m_viewPortDim;

	TTF_Font* defaultFont = nullptr;
};