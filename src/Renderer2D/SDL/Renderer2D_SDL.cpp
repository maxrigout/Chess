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
	SDL_GetWindowSize(SDL_RenderGetWindow(m_renderer), &m_windowDim.x, &m_windowDim.y);
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

void Renderer2D_SDL::DrawDisk(point2d<int> center, int radius, const Color& color) const
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
		point2d<int> left{center.x - x, rect.y + y};
		point2d<int> right{center.x + x, rect.y + y};
		SDL_RenderDrawLine(m_renderer, left.x, left.y, right.x, right.y);
	}
}

void Renderer2D_SDL::DrawCircle(point2d<int> center, int radius, const Color& color) const
{
	SetRenderDrawColor(color);
}

void Renderer2D_SDL::FillCircle(point2d<int> center, int radius, const Color& color) const
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
		point2d<int> left{center.x - x, rect.y + y};
		point2d<int> right{center.x + x, rect.y + y};
		SDL_RenderDrawLine(m_renderer, left.x, left.y, right.x, right.y);
	}
}

void Renderer2D_SDL::DrawRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{ position.x, position.y, dimensions.w, dimensions.h };
	SDL_RenderDrawRect(m_renderer, &rect);
}

void Renderer2D_SDL::FillRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Rect rect{ position.x, position.y, dimensions.w, dimensions.h };
	SDL_RenderFillRect(m_renderer, &rect);
}

void Renderer2D_SDL::DrawLine(point2d<int> start, point2d<int> end, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);
}

void Renderer2D_SDL::DrawText(point2d<int> position, const std::string& text, const Color& color) const
{
	// TODO: refactor...
	// 	cache the messages that are drawn frequently
	//	add option to specify the offset
	point2d<int> offset{20, 20};
	SetRenderDrawColor(color);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(defaultFont, text.c_str(), toSDL_Color(color));
	SDL_Texture* message = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
	SDL_Rect dest{ position.x + offset.x, position.y + offset.y, m_cellDim.w - 2 * offset.x, m_cellDim.h - 2 * offset.y };

	SDL_RenderCopy(m_renderer, message, NULL, &dest);

	// TODO: store the text in a map
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}


vector2d<int> Renderer2D_SDL::GetWindowDim() const
{
	return m_windowDim;
}
vector2d<int> Renderer2D_SDL::GetCellDim() const
{
	return m_cellDim;
}
vector2d<int> Renderer2D_SDL::GetViewPortDim() const
{
	return m_viewPortDim;
}
void Renderer2D_SDL::SetWindowDim(vector2d<int> dim)
{
	m_windowDim = dim;
}
void Renderer2D_SDL::SetCellDim(vector2d<int> dim)
{
	m_cellDim = dim;
}
void Renderer2D_SDL::SetViewPortDim(vector2d<int> dim)
{
	m_viewPortDim = dim;
}