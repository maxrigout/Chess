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
	virtual void DrawDisk(const pt2di& center, int radius, const Color& color) const;
	virtual void DrawCircle(const pt2di& center, int radius, const Color& color) const;
	virtual void FillCircle(const pt2di& center, int radius, const Color& color) const;
	virtual void DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const;
	virtual void FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const;
	virtual void DrawLine(const pt2di& start, const pt2di& end, const Color& color) const;
	virtual void DrawText(const pt2di& position, const std::string& text, const Color& color) const;
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const;

	virtual const vec2di& GetWindowDim() const;
	virtual const vec2di& GetCellDim() const;
	virtual const vec2di& GetViewPortDim() const;
	virtual void SetWindowDim(const vec2di& dim);
	virtual void SetCellDim(const vec2di& dim);
	virtual void SetViewPortDim(const vec2di& dim);

private:
	void SetRenderDrawColor(const Color& c) const { SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a); }

	SDL_Renderer* m_renderer;
	vec2di m_windowDim;
	vec2di m_cellDim;
	vec2di m_viewPortDim;

	TTF_Font* defaultFont = nullptr;
};