#pragma once

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "Renderer2D/Renderer2D.h"

#include <unordered_map>

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
	virtual void DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const;
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const;

	virtual SpriteID LoadTexture(const char* path);
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& sprite);
	virtual void DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const;
	virtual void DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const;

	virtual const vec2di& GetWindowDim() const;
	virtual const vec2di& GetCellDim() const;
	virtual const vec2di& GetViewPortDim() const;
	virtual void SetWindowDim(const vec2di& dim);
	virtual void SetCellDim(const vec2di& dim);
	virtual void SetViewPortDim(const vec2di& dim);

private:
	void SetRenderDrawColor(const Color& c) const { SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a); }

	struct Sprite
	{
		int textureIndex;
		pt2di topLeft;
		vec2di size;
	};

	SDL_Renderer* m_renderer;
	vec2di m_windowDim;
	vec2di m_cellDim;
	vec2di m_viewPortDim;

	TTF_Font* defaultFont = nullptr;

	std::vector<SDL_Texture*> m_textures;
	std::unordered_map<std::string, SpriteID> m_tagsMap;
	std::vector<Sprite> m_sprites;
};