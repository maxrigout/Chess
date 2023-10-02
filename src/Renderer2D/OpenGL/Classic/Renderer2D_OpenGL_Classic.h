#pragma once

#include "Renderer2D/OpenGL/Renderer2D_OpenGL.h"

#include <string>
#include <functional>
#include <set>
#include <vector>
#include <unordered_map>

#define N_QUAD_VERTICES 4

class Renderer2D_OpenGL_Classic : public Renderer2D_OpenGL
{
public:
	Renderer2D_OpenGL_Classic();
	virtual ~Renderer2D_OpenGL_Classic();

	virtual void Begin() override;
	virtual void End() override;
	virtual void Clear(const Color& color) const override;
	virtual void DrawDisk(const pt2di& center, int radius, const Color& color) const override {}
	virtual void DrawCircle(const pt2di& center, int radius, const Color& color) const override {}
	virtual void FillCircle(const pt2di& center, int radius, const Color& color) const override {}
	virtual void DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override {}
	virtual void FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override {}
	virtual void DrawLine(const pt2di& start, const pt2di& end, const Color& color) const override {}
	virtual void DrawText(const pt2di& position, const std::string& text, const Color& color) const override {}
	virtual void DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const override {}
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const override {}
	
	virtual SpriteID LoadSprite(const SpriteDescriptor& spriteDescriptor) override;
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) override;
	virtual SpriteID LoadTexture(const char* path, const std::string& tag) override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const override;
	
	virtual vec2di GetSpriteSize(const std::string& spriteTag) const override;
	virtual const vec2di& GetCellDim() const override;
	virtual const vec2di& GetViewPortDim() const override;
	virtual void SetCellDim(const vec2di& dim) override;
	virtual void SetViewPortDim(const vec2di& dim) override;

	virtual void OnRenderEnd(std::function<void(void)>&& fn) override
	{
		m_onRenderEnd = std::move(fn);
	}

	struct Vertex
	{
		pt2df position;
		Color color;
		pt2df textureCoords;
	};

	struct Renderable
	{
		unsigned int VAO = -1, VBO = -1, EBO = -1;
		size_t nElements = 0;
	};

	struct Texture
	{
		// the opengl id
		unsigned int id;
		vec2di dim;
		std::string path;
	};

	struct Sprite
	{
		// the internal texture id... (ie the index in the texture array)
		std::string textureId;
		pt2df topLeft;
		pt2df bottomRight;
	};


private:
	bool IsValidSpriteId(SpriteID spriteId) const { return m_sprites.size() > spriteId; }
	void Flush();
	Texture& GetTexture(const std::string& path);

	std::function<void(void)> m_onRenderEnd;

	std::unordered_map<std::string, Texture> m_textures;
	std::vector<Sprite> m_sprites;
	std::unordered_map<std::string, size_t> m_tagsMap;

	Renderable m_quadRenderable;

	int m_shader = -1;
	int m_uniformOffsetLoc = -1;
	int m_uniformScaleLoc = -1;
	int m_uniformTextureIdLoc = -1;
	int m_uniformQuadTextureCoordsLoc[N_QUAD_VERTICES];

	vec2di m_windowDim{};
	vec2di m_cellDim{};
	vec2di m_viewPortDim{};
};
