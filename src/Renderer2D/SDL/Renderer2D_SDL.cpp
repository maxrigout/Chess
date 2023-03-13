#include "Renderer2D_SDL.h"
#include "SDL2_image/SDL_image.h"

#include "Assets.h"
#include "Core/Logger.h"

static SDL_Color toSDL_Color(const Color& color)
{
	PixelColor pixelColor = color.ToPixelColor();
	SDL_Color c;
	c.a = pixelColor.a;
	c.b = pixelColor.b;
	c.g = pixelColor.g;
	c.r = pixelColor.r;
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
		LOG_ERROR(std::string("Error initializing SDL_ttf: ") + TTF_GetError());

	if (IMG_Init(IMG_INIT_JPG) < 0)
		LOG_ERROR(std::string("Error initializing SDL_image: ") + IMG_GetError());

	defaultFont = TTF_OpenFont(FONT_PATH, 64);
	// defaultFont = TTF_OpenFont("Fonts/VirtualNote.ttf", 64);
	if (defaultFont == nullptr)
	{
		LOG_ERROR("cannot open font");
	}
}

Renderer2D_SDL::~Renderer2D_SDL()
{
	for (auto& texture : m_textures)
	{
		SDL_DestroyTexture(texture.texture);
	}
	TTF_CloseFont(defaultFont);
	IMG_Quit();
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

void Renderer2D_SDL::DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const
{
	SetRenderDrawColor(color);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(defaultFont, text.c_str(), toSDL_Color(color));
	SDL_Texture* message = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
	// SDL_Rect dest{ topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y };
	SDL_Rect dest{ topLeft.x, topLeft.y, dimensions.x, dimensions.y };

	SDL_RenderCopy(m_renderer, message, NULL, &dest);

	// TODO: store the text in a map
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

void Renderer2D_SDL::DrawArrow(const pt2di& screenStart, const pt2di& screenEnd, const Color& color) const
{
	if (screenStart.x < 0 || screenStart.y < 0)
		return;
	if (screenEnd.x < 0 || screenEnd.y < 0)
		return;
	const double headBaseSize = 30.0;
	const double headHeight = 30.0;
	// pt2dd screenStart(start.x * m_cellDim.w + m_cellDim.w / 2, start.y * m_cellDim.h + m_cellDim.h / 2), 
	// 	screenEnd(end.x * m_cellDim.w + m_cellDim.w / 2, end.y * m_cellDim.h + m_cellDim.h / 2);
	vec2dd arrowVector = screenEnd - screenStart;
	vec2dd arrowDir = arrowVector / arrowVector.Norm();
	// go back by headHeight units:
	pt2dd arrowBase = screenEnd - (arrowDir * headHeight);
	// rotate by 90 degrees
	/*
	(cos, -sin) -> (0, -1)
	(sin,  cos)    (1,  0)
	*/
	vec2dd headBaseDir = { -arrowDir.y, arrowDir.x };
	// move by headBaseSize units
	pt2di left = arrowBase + headBaseDir * headBaseSize / 2;
	pt2di right = arrowBase - headBaseDir * headBaseSize / 2;

	SetRenderDrawColor(color);
	SDL_RenderDrawLine(m_renderer, screenStart.x, screenStart.y, screenEnd.x, screenEnd.y);
	SDL_RenderDrawLine(m_renderer, screenEnd.x, screenEnd.y, left.x, left.y);
	SDL_RenderDrawLine(m_renderer, screenEnd.x, screenEnd.y, right.x, right.y);
}


Renderer2D::SpriteID Renderer2D_SDL::LoadTexture(const char* path, const std::string& tag)
{
	SDL_Texture* texture = IMG_LoadTexture(m_renderer, path);
	int width, height;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	m_textures.push_back({ texture, { width, height } });
	int textureId = m_textures.size() - 1;
	m_sprites.push_back({ textureId, { 0, 0 }, { width, height } });
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(tag, spriteId);
	return spriteId;
}

std::vector<Renderer2D::SpriteID> Renderer2D_SDL::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	std::vector<SpriteID> result;
	SDL_Texture* sheet = IMG_LoadTexture(m_renderer, path);
	int width, height;
	SDL_QueryTexture(sheet, NULL, NULL, &width, &height);
	m_textures.push_back({ sheet, { width, height } });
	int textureId = m_textures.size() - 1;
	for (const auto& sprite : spriteDescriptors)
	{
		pt2di offset = sprite.offset;
		vec2di size = sprite.size;
		switch (sprite.offsetType)
		{
		case SpriteOffsetType::TopLeft:
			break;
		case SpriteOffsetType::TopRight:
			offset.x -= size.w;
			break;
		case SpriteOffsetType::BottomLeft:
			offset.y -= size.h;
			break;
		case SpriteOffsetType::BottomRight:
			offset = offset - size;
			break;
		}
		m_sprites.push_back({ textureId, offset, size });
		m_tagsMap.emplace(sprite.tag, m_sprites.size() - 1);
		result.push_back(m_sprites.size() - 1);
	}
	return result;
}

bool Renderer2D_SDL::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const Renderer2D::SpriteID& spriteId) const
{
	const Sprite& sprite = m_sprites[spriteId];
	const Texture& texture = m_textures[sprite.textureIndex];
	SDL_Rect src, dest;
	src.x = sprite.topLeft.x;
	src.y = sprite.topLeft.y;
	src.w = sprite.size.w > 0 ? sprite.size.w : texture.size.w;
	src.h = sprite.size.h > 0 ? sprite.size.h : texture.size.h;

	dest.x = topLeft.x;
	dest.y = topLeft.y;
	dest.w = dimensions.w;
	dest.h = dimensions.h;

	return SDL_RenderCopy(m_renderer, texture.texture, &src, &dest) == 0;
}

bool Renderer2D_SDL::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const
{
	try
	{
		const SpriteID& spriteId = m_tagsMap.at(tag);
		return DrawSprite(topLeft, dimensions, spriteId);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("an error has occured with tag: " + tag + " - " + e.what());
	}
	return false;
}


vec2di Renderer2D_SDL::GetSpriteSize(const std::string& spriteTag) const
{
	try
	{
		const SpriteID& spriteId = m_tagsMap.at(spriteTag);
		return m_textures[spriteId].size;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("an error has occured with tag: " + spriteTag + " - " + e.what());
	}
	return { -1, -1 };
}

const vec2di& Renderer2D_SDL::GetCellDim() const
{
	return m_cellDim;
}
const vec2di& Renderer2D_SDL::GetViewPortDim() const
{
	return m_viewPortDim;
}
void Renderer2D_SDL::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}
void Renderer2D_SDL::SetViewPortDim(const vec2di& dim)
{
	m_viewPortDim = dim;
}
