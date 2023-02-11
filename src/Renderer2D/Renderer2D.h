#pragma once

#include <string>

#include "Math/point2d.h"
#include "Math/vector2d.h"

struct Color
{
	uint8_t r, g, b, a = 255;
};

static const Color WHITE{255, 255, 255, 255};
static const Color RED{255, 0, 0, 255};
static const Color GREEN{0, 255, 0, 255};
static const Color BLUE{0, 0, 255, 255};
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
static const Color VERY_DARK_GREEN{0, 63, 0, 255};
static const Color VERY_DARK_BLUE{0, 0, 63, 255};
static const Color VERY_DARK_GREY{50, 50, 50, 255};
static const Color VERY_DARK_YELLOW{63, 63, 0, 255};
static const Color VERY_DARK_CYAN{0, 63, 63, 255};
static const Color VERY_DARK_MAGENTA{63, 0, 63, 255};

class Renderer2D
{
public:
	virtual ~Renderer2D() = default;

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Clear(const Color& color) const = 0;
	virtual void DrawDisk(point2d<int> center, int radius, const Color& color) const = 0;
	virtual void DrawCircle(point2d<int> center, int radius, const Color& color) const = 0;
	virtual void FillCircle(point2d<int> center, int radius, const Color& color) const = 0;
	virtual void DrawRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const = 0;
	virtual void FillRect(point2d<int> position, vector2d<int> dimensions, const Color& color) const = 0;
	virtual void DrawLine(point2d<int> start, point2d<int> end, const Color& color) const = 0;
	virtual void DrawText(point2d<int> position, const std::string& text, const Color& color) const = 0;

	virtual vector2d<int> GetWindowDim() const = 0;
	virtual vector2d<int> GetCellDim() const = 0;
	virtual vector2d<int> GetViewPortDim() const = 0;
	virtual void SetWindowDim(vector2d<int> dim) = 0;
	virtual void SetCellDim(vector2d<int> dim) = 0;
	virtual void SetViewPortDim(vector2d<int> dim) = 0;
};