#include "Renderer2D_SDL.h"

#include <iostream>

SDL_Color toSDL_Color(const Color& color)
{
	SDL_Color c;
	c.a = color.a;
	c.b = color.b;
	c.g = color.g;
	c.r = color.r;
	return c;
}

Renderer2D_SDL::Renderer2D_SDL(SDL_Renderer* renderer)
	: m_renderer(renderer)
{
	SDL_Rect viewPortRect;
	SDL_RenderGetViewport(m_renderer, &viewPortRect);
	SDL_GetWindowSize(SDL_RenderGetWindow(m_renderer), &m_windowDim.w, &m_windowDim.h);
	m_viewPortDim = { viewPortRect.w, viewPortRect.h };
	if (TTF_Init() < 0)
		std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;

	defaultFont = TTF_OpenFont("fonts/Vanilla Caramel.otf", 64);
	// defaultFont = TTF_OpenFont("Fonts/VirtualNote.ttf", 64);
	if (defaultFont == nullptr)
	{
		std::cout << "cannot open font\n";
	}
}

Renderer2D_SDL::~Renderer2D_SDL()
{
	TTF_CloseFont(defaultFont);
	TTF_Quit();
	SDL_DestroyRenderer(m_renderer);
}

void Renderer2D_SDL::Begin()
{

}

void Renderer2D_SDL::End()
{
	SDL_RenderPresent(m_renderer);
}

void Renderer2D_SDL::Clear(const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_RenderClear(m_renderer);
}

void Renderer2D_SDL::DrawDisk(const pt2di& center, int radius, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
	int prev_cos = 0;
	for (int y = 0; y < rect.h; ++y)
	{
		float sin = ((float)y / rect.h) * 2.0f - 1.0f;
		float sin2 = sin * sin;
		float cos = prev_cos; 
		float cos2 = cos * cos; // start at the revious pos
		while (sin2 + cos2 < 1)
		{
			cos += (1.0f / rect.w / 2.0f);
			cos2 = cos * cos;
		}
		int x = radius * sqrt(cos2);
		pt2di left{center.x - x, rect.y + y};
		pt2di right{center.x + x, rect.y + y};
		SDL_RenderDrawLine(m_renderer, left.x, left.y, right.x, right.y);
	}
}

void Renderer2D_SDL::DrawCircle(const pt2di& center, int radius, const Color& color) const
{
	SetRenderDrawColor(color);
}

void Renderer2D_SDL::FillCircle(const pt2di& center, int radius, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
	for (int y = 0; y < rect.h; ++y)
	{
		// find the 2 extreme points on the circle by scanning down
		float sin = ((float)y / rect.h) * 2.0f * radius - radius;
		float sin2 = (sin * sin) / radius / radius;
		float cos2 = 1 - sin2;
		int x = radius * sqrt(cos2);
		pt2di left{center.x - x, rect.y + y};
		pt2di right{center.x + x, rect.y + y};
		SDL_RenderDrawLine(m_renderer, left.x, left.y, right.x, right.y);
	}
}

void Renderer2D_SDL::DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{ position.x, position.y, dimensions.w, dimensions.h };
	SDL_RenderDrawRect(m_renderer, &rect);
}

void Renderer2D_SDL::FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{ position.x, position.y, dimensions.w, dimensions.h };
	SDL_RenderFillRect(m_renderer, &rect);
}

void Renderer2D_SDL::DrawLine(const pt2di& start, const pt2di& end, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);
}

void Renderer2D_SDL::DrawText(const pt2di& position, const std::string& text, const Color& color) const
{
	// TODO: refactor...
	// 	cache the messages that are drawn frequently
	//	add option to specify the offset
	pt2di offset{20, 20};
	SetRenderDrawColor(color);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(defaultFont, text.c_str(), toSDL_Color(color));
	SDL_Texture* message = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
	SDL_Rect dest{ position.x + offset.x, position.y + offset.y, m_cellDim.w - 2 * offset.x, m_cellDim.h - 2 * offset.y };

	SDL_RenderCopy(m_renderer, message, NULL, &dest);

	// TODO: store the text in a map
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

void Renderer2D_SDL::DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const
{
	double percent_head = 0.8;
	double head_width = 3000.0;
	pt2di head_left, head_right;
	double ss_x = start.x * m_cellDim.w + m_cellDim.w * 0.5f;
	double ss_y = start.y * m_cellDim.h + m_cellDim.h * 0.5f;
	double se_x = end.x * m_cellDim.w + m_cellDim.w * 0.5f;
	double se_y = end.y * m_cellDim.h + m_cellDim.h * 0.5f;
	pt2df screenStart(ss_x, ss_y), screenEnd(se_x, se_y);
	auto PointOnLine = [&](double t)
	{
		double x = ss_x * (1.0 - t) + se_x * t;
		double y = ss_y * (1.0 - t) + se_y * t;
		return pt2df(x, y);
	};

	head_left = PointOnLine(percent_head) + ((screenEnd - screenStart).Normalize().Rotate90() * head_width);
	head_right = PointOnLine(percent_head) + ((screenEnd - screenStart).Normalize().Rotate270() * head_width);

	DrawLine({ ss_x, ss_y }, { se_x, se_y }, color);
	DrawLine({ se_x, se_y} , { head_left.x, head_left.y }, color);
	DrawLine({ se_x, se_y} , { head_right.x, head_right.y }, color);
}

const vec2di& Renderer2D_SDL::GetWindowDim() const
{
	return m_windowDim;
}
const vec2di& Renderer2D_SDL::GetCellDim() const
{
	return m_cellDim;
}
const vec2di& Renderer2D_SDL::GetViewPortDim() const
{
	return m_viewPortDim;
}
void Renderer2D_SDL::SetWindowDim(const vec2di& dim)
{
	m_windowDim = dim;
}
void Renderer2D_SDL::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}
void Renderer2D_SDL::SetViewPortDim(const vec2di& dim)
{
	m_viewPortDim = dim;
}