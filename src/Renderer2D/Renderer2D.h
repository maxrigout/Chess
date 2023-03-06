#pragma once

#include "Math/point2d.h"
#include "Math/vector2d.h"

#include <string>
#include <vector>

enum class RendererBackendType
{
	Unspec,
	OpenGL,
	Vulkan,
	Metal,
	SDL2,
};

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

enum class SpriteOffsetType
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

struct SpriteDescriptor
{
	pt2di offset{-1, -1};
	vec2di size{-1, -1};
	SpriteOffsetType type = SpriteOffsetType::TopLeft;
	std::string tag = "";
};

class Renderer2D
{
public:
	using SpriteID = size_t;

	virtual ~Renderer2D() = default;

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Clear(const Color& color) const = 0;
	virtual void DrawDisk(const pt2di& center, int radius, const Color& color) const = 0;
	virtual void DrawCircle(const pt2di& center, int radius, const Color& color) const = 0;
	virtual void FillCircle(const pt2di& center, int radius, const Color& color) const = 0;
	virtual void DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const = 0;
	virtual void FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const = 0;
	virtual void DrawLine(const pt2di& start, const pt2di& end, const Color& color) const = 0;
	virtual void DrawText(const pt2di& position, const std::string& text, const Color& color) const = 0;
	virtual void DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const = 0;
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const = 0;
	
	virtual SpriteID LoadTexture(const char* path, const std::string& tag) = 0;
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& sprite) = 0;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const = 0;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const = 0;
	
	virtual vec2di GetSpriteSize(const std::string& spriteTag) const = 0;
	virtual const vec2di& GetWindowDim() const = 0;
	virtual const vec2di& GetCellDim() const = 0;
	virtual const vec2di& GetViewPortDim() const = 0;
	virtual void SetWindowDim(const vec2di& dim) = 0;
	virtual void SetCellDim(const vec2di& dim) = 0;
	virtual void SetViewPortDim(const vec2di& dim) = 0;
};
