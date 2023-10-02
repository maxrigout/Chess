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

	virtual void Begin() override;
	virtual void End() override;
	virtual void Clear(const Color& color) const override;
	virtual void DrawDisk(const pt2di& center, int radius, const Color& color) const override;
	virtual void DrawCircle(const pt2di& center, int radius, const Color& color) const override;
	virtual void FillCircle(const pt2di& center, int radius, const Color& color) const override;
	virtual void DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override;
	virtual void FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override;
	virtual void DrawLine(const pt2di& start, const pt2di& end, const Color& color) const override;
	virtual void DrawText(const pt2di& position, const std::string& text, const Color& color) const override;
	virtual void DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const override;
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const override;

	virtual SpriteID LoadSprite(const SpriteDescriptor& spriteDescriptor) override { return -1; }
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) override;
	virtual SpriteID LoadTexture(const char* path, const std::string& tag) override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const override;

	virtual vec2di GetSpriteSize(const std::string& spriteTag) const override;
	virtual const vec2di& GetCellDim() const override;
	virtual const vec2di& GetViewPortDim() const override;
	virtual void SetCellDim(const vec2di& dim) override;
	virtual void SetViewPortDim(const vec2di& dim) override;

private:
	void SetRenderDrawColor(const Color& color) const
	{
		PixelColor c = color.ToPixelColor();
		SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	}

	struct Sprite
	{
		int textureIndex;
		pt2di topLeft;
		vec2di size;
	};

	struct Texture
	{
		SDL_Texture* texture;
		vec2di size;
	};

	SDL_Renderer* m_renderer;
	vec2di m_windowDim;
	vec2di m_cellDim;
	vec2di m_viewPortDim;

	TTF_Font* defaultFont = nullptr;

	std::vector<Texture> m_textures;
	std::unordered_map<std::string, SpriteID> m_tagsMap;
	std::vector<Sprite> m_sprites;
};
