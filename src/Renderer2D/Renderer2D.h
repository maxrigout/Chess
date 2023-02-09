#pragma once
#include "Math/point2d.h"
#include "Math/vector2d.h"
#include <SDL2/SDL.h>

struct Color
{
	uint8_t r, g, b, a = 255;
};

static const Color WHITE{255, 255, 255, 255};
static const Color RED{255, 0, 0, 255};
static const Color GREEN{255, 0, 0, 255};
static const Color BLUE{255, 0, 0, 255};
static const Color BLACK{0, 0, 0, 255};
static const Color GREY{200, 200, 200, 255};
static const Color YELLOW{255, 255, 0, 255};
static const Color CYAN{0, 255, 255, 255};
static const Color MAGENTA{255, 0, 255, 255};
static const Color PURPLE{100, 100, 200, 255};
static const Color PURPLE2{127, 127, 255, 255};

static const Color DARK_RED{127, 0, 0, 255};
static const Color DARK_GREEN{0, 127, 0, 255};
static const Color DARK_BLUE{0, 0, 127, 255};
static const Color DARK_GREY{100, 100, 100, 255};
static const Color DARK_YELLOW{127, 127, 0, 255};
static const Color DARK_CYAN{0, 127, 127, 255};
static const Color DARK_MAGENTA{127, 0, 127, 255};
static const Color DARK_PURPLE{50, 50, 100, 255};
static const Color DARK_PURPLE2{63, 63, 127, 255};

static const Color VERY_DARK_RED{63, 0, 0, 255};
static const Color VERY_DARK_GREEN{63, 0, 0, 255};
static const Color VERY_DARK_BLUE{63, 0, 0, 255};
static const Color VERY_DARK_GREY{50, 50, 50, 255};
static const Color VERY_DARK_YELLOW{63, 63, 0, 255};
static const Color VERY_DARK_CYAN{0, 63, 63, 255};
static const Color VERY_DARK_MAGENTA{63, 0, 63, 255};

struct Renderer2D
{
	SDL_Renderer* renderer;
	int windowWidth;
	int windowHeight;
	int cellWidth;
	int cellHeight;
	void DrawCircle(point2d<int> center, int radius, const Color& color) const
	{

	}
	void FillCircle(point2d<int> center, int radius, const Color& color) const
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_Rect rect{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
		for (int y = 0; y < rect.h; ++y)
		{
			// find the 2 extreme points on the circle
			float cosy = ((float)y / rect.h) * 2.0f * radius - radius;
			float cos2y = (cosy * cosy) / radius / radius;
			float cos2x = 1 - cos2y;
			int x = radius * sqrt(cos2x);
			point2d<int> left{center.x - x, rect.y + y};
			point2d<int> right{center.x + x, rect.y + y};
			SDL_RenderDrawLine(renderer, left.x, left.y, right.x, right.y);
		}
	}
	void DrawDisk(point2d<int> center, int radius, const Color& color) const
	{
		return FillCircle(center, radius, color);
	}
};