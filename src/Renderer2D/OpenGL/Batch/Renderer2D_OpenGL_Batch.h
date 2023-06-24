#pragma once

#include "Renderer2D/Renderer2D.h"

#include <string>
#include <functional>
#include <set>
#include <vector>
#include <unordered_map>

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

class Renderer2D_OpenGL_Batch : public Renderer2D
{
public:
	static void LoadOpenGLLibrary(void*(procAddr)(const char*));

	Renderer2D_OpenGL_Batch();
	virtual ~Renderer2D_OpenGL_Batch();

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
	
	virtual SpriteID LoadTexture(const char* path, const std::string& tag) override;
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const override;
	
	virtual vec2di GetSpriteSize(const std::string& spriteTag) const override;
	// virtual const vec2di& GetWindowDim() const override;
	virtual const vec2di& GetCellDim() const override;
	virtual const vec2di& GetViewPortDim() const override;
	// virtual void SetWindowDim(const vec2di& dim) override;
	virtual void SetCellDim(const vec2di& dim) override;
	virtual void SetViewPortDim(const vec2di& dim) override;

	template <typename Fn>
	void OnRenderEnd(Fn&& fn)
	{
		m_onRenderEnd = std::move(fn);
	}

	struct Texture
	{
		unsigned int id;
		vec2di dim;
	};

	struct Sprite
	{
		size_t textureId;
		pt2df topLeft;
		pt2df bottomRight;
	};


private:
	bool IsValidSpriteId(SpriteID spriteId) const { return m_sprites.size() > spriteId; }
	void Flush();

	std::function<void(void)> m_onRenderEnd;

	std::vector<Texture> m_textures;
	std::vector<Sprite> m_sprites;
	std::unordered_map<std::string, size_t> m_tagsMap;

	Renderable m_quadRenderable;
	mutable std::vector<Vertex> m_data;
	mutable std::vector<unsigned int> m_indices;

	int m_shader = -1;

	vec2di m_windowDim{};
	vec2di m_cellDim{};
	vec2di m_viewPortDim{};
};
