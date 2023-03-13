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

struct PixelColor
{
	uint8_t r, g, b, a;
};

struct Color
{
	float r, g, b, a;
	PixelColor ToPixelColor() const { return {(uint8_t)(255 * r), (uint8_t)(255 * g), (uint8_t)(255 * b), (uint8_t)(255 * a) }; }

};

static const Color WHITE{ 1.0f, 1.0f, 1.0f, 1.0f };
static const Color RED{ 1.0f, 0.0f, 0.0f, 1.0f };
static const Color GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
static const Color BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };
static const Color BLACK{ 0.0f, 0.0f, 0.0f, 1.0f };
static const Color GREY{ 0.7843f, 0.7843f, 0.7843f, 1.0f };
static const Color YELLOW{ 1.0f, 1.0f, 0.0f, 1.0f };
static const Color CYAN{ 0.0f, 1.0f, 1.0f, 1.0f };
static const Color MAGENTA{ 1.0f, 0.0f, 1.0f, 1.0f };
static const Color PURPLE{ 0.392157f, 0.392157f, 0.7843f, 1.0f };
static const Color PURPLE2{ 0.5f, 0.5f, 1.0f, 1.0f };

static const Color DARK_RED{ 0.5f, 0.0f, 0.0f, 1.0f };
static const Color DARK_GREEN{ 0.0f, 0.5f, 0, 1.0f };
static const Color DARK_BLUE{ 0.0f, 0.0f, 0.5f, 1.0f };
static const Color DARK_GREY{ 0.392157f, 0.392157f, 0.392157f, 1.0f };
static const Color DARK_YELLOW{ 0.5f, 0.5f, 0.0f, 1.0f };
static const Color DARK_CYAN{ 0.0f, 0.5f, 0.5f, 1.0f };
static const Color DARK_MAGENTA{ 0.5f, 0.0f, 0.5f, 1.0f };
static const Color DARK_PURPLE{ 0.1991f, 0.1991f, 0.392157f, 1.0f };
static const Color DARK_PURPLE2{ 0.2471f, 0.2471f, 0.5f, 1.0f };

static const Color VERY_DARK_RED{ 0.2471f, 0.0f, 0.0f, 1.0f };
static const Color VERY_DARK_GREEN{ 0, 0.2471f, 0.0f, 1.0f };
static const Color VERY_DARK_BLUE{ 0.0f, 0.0f, 0.2471f, 1.0f };
static const Color VERY_DARK_GREY{ 0.1991f, 0.1991f, 0.1991f, 1.0f };
static const Color VERY_DARK_YELLOW{ 0.2471f, 0.2471f, 0.0f, 1.0f };
static const Color VERY_DARK_CYAN{ 0.0f, 0.2471f, 0.2471f, 1.0f };
static const Color VERY_DARK_MAGENTA{ 0.2471f, 0.0f, 0.2471f, 1.0f };

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
	SpriteOffsetType offsetType = SpriteOffsetType::TopLeft;
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
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) = 0;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const = 0;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const = 0;
	
	virtual vec2di GetSpriteSize(const std::string& spriteTag) const = 0;
	virtual const vec2di& GetCellDim() const = 0;
	virtual const vec2di& GetViewPortDim() const = 0;
	virtual void SetCellDim(const vec2di& dim) = 0;
	virtual void SetViewPortDim(const vec2di& dim) = 0;
};
