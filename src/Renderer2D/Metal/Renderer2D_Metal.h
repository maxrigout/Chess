#pragma once
#include "Renderer2D/Renderer2D.h"
#ifdef SUPPORT_METAL

#include <string>
#include <functional>
#include <set>
#include <vector>
#include <unordered_map>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/simd.h>

#define N_QUAD_VERTICES 4

class Renderer2D_Metal : public Renderer2D
{
public:
	Renderer2D_Metal(void* swapchain, int width, int height);
	virtual ~Renderer2D_Metal();

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
	
	virtual SpriteID LoadTexture(const char* path, const std::string& tag) override;
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const override;
	
	virtual vec2di GetSpriteSize(const std::string& spriteTag) const override;
	virtual const vec2di& GetCellDim() const override;
	virtual const vec2di& GetViewPortDim() const override;
	virtual void SetCellDim(const vec2di& dim) override;
	virtual void SetViewPortDim(const vec2di& dim) override;

	struct Vertex
	{
		simd::float2 position;
		simd::float4 color;
		simd::float2 textureCoords;
	};

	struct Texture
	{
		NS::SharedPtr<MTL::Texture> texture;
		vec2di dim;
		std::string path;
	};

	struct Sprite
	{
		// the internal texture id... (ie the index in the texture array)
		size_t textureId;
		pt2df topLeft;
		pt2df bottomRight;
	};

	struct Renderable
	{
		MTL::Buffer* pVertexBuffer = nullptr;
		MTL::Buffer* pIndexBuffer = nullptr;
		size_t nVertices = 0;
	};


private:
	Renderable CreateRenderable(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	void BuildShaders();
	void BuildBuffers();

	bool IsValidSpriteId(SpriteID spriteId) const { return m_sprites.size() > spriteId; }

	std::vector<Texture> m_textures;
	std::vector<Sprite> m_sprites;
	std::unordered_map<std::string, size_t> m_tagsMap;

	NS::AutoreleasePool* m_pGlobalAutoreleasePool = nullptr;

	// MTK::View* m_pView = nullptr;
	CA::MetalLayer* m_pSwapchain = nullptr;
	MTL::Device* m_pDevice = nullptr;
	NS::AutoreleasePool* m_pAutoReleasePool = nullptr;
	MTL::CommandQueue* m_pCommandQueue = nullptr;
	MTL::RenderPipelineState* m_pRenderPipelineState = nullptr;
	MTL::Buffer* m_pVertexBuffer = nullptr;
	MTL::Buffer* m_pIndexBuffer = nullptr;
	// MTL::Buffer* m_pUniformBuffer = nullptr;

	MTL::RenderPassDescriptor* m_pRenderPassDescriptor = nullptr;
	MTL::RenderPassColorAttachmentDescriptor* m_pColorAttachment = nullptr;

	CA::MetalDrawable* m_pDrawable = nullptr;
	MTL::CommandBuffer* m_pCmd = nullptr;
	MTL::RenderCommandEncoder* m_pEnc = nullptr;

	dispatch_semaphore_t m_semaphore;

	CGRect m_viewPort;

	Renderable m_quadRenderable;

	vec2di m_windowDim{};
	vec2di m_cellDim{};
	vec2di m_viewPortDim{};
};
#endif