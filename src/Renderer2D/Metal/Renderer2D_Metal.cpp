#include "Renderer2D_Metal.h"
#ifdef SUPPORT_METAL
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>


#include "Core/Logger.h"

#include <iostream>
#include <string>

#include <stb/stb_image.h>

#define PRIMITIVE_RESTART_INDEX 0xFFFFFFFF

static const char* shaderSrc = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
	float4 position [[position]];
	half4 color;
};
struct VertexIn
{
	float2 position [[attribute(0)]];
	float4 color [[attribute(1)]];
};

VertexOut vertex vertexMain(VertexIn v [[stage_in]] )
{
	VertexOut out;
	out.position = float4(v.position, 1.0, 1.0);
	out.color = half4(v.color);
	return out;
}

half4 fragment fragmentMain( VertexOut in [[stage_in]] )
{
	return in.color;
}
)";

// static const int maxTexturesBind = 4;

/*
	0 ------ 3
	|        |
	|        |
	1 ------ 2
*/

static const std::vector<Renderer2D_Metal::Vertex> quadVertices = {
	{ { 0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 1.0f,  0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
};

static const std::vector<unsigned int> quadIndices {
	0, 1, 2,
	0, 2, 3
};

//   tl_x, tl_y, br_x, br_y
static const float quadWeights[N_QUAD_VERTICES][4] = {
	{ 1, 1, 0, 0 },
	{ 1, 0, 0, 1 },
	{ 0, 0, 1, 1 },
	{ 0, 1, 1, 0 },
};

Renderer2D_Metal::Renderable Renderer2D_Metal::CreateRenderable(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	Renderer2D_Metal::Renderable renderable;
	renderable.pVertexBuffer = m_pDevice->newBuffer(vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeManaged);
	memcpy(renderable.pVertexBuffer->contents(), vertices.data(), vertices.size() * sizeof(Vertex));
	renderable.pVertexBuffer->didModifyRange(NS::Range::Make(0, renderable.pVertexBuffer->length()));

	renderable.pIndexBuffer = m_pDevice->newBuffer(indices.size() * sizeof(unsigned int), MTL::ResourceStorageModeManaged);
	memcpy(renderable.pIndexBuffer->contents(), indices.data(), indices.size() * sizeof(unsigned int));
	renderable.pIndexBuffer->didModifyRange(NS::Range::Make(0, renderable.pIndexBuffer->length()));

	return renderable;
}

static Renderer2D_Metal::Texture LoadTextureToGPU(const char* path)
{
	int width, height, nComponents;
	unsigned char* imageData = stbi_load(path, &width, &height, &nComponents, 0);
	if (!imageData)
	{
		// cannot load image...
		stbi_image_free(imageData);

		return {(unsigned int)-1, {-1, -1}};
	}

	unsigned int texture = -1;

	// TODO

	stbi_image_free(imageData);

	const size_t maxMsgSz = 500;
	char message[maxMsgSz] = { 0 };

	LOG_DEBUG(message);

	return { texture, { width, height }, std::string(path) };
}

Renderer2D_Metal::Renderer2D_Metal(void* sp, int width, int height)
{
	m_pDevice = MTL::CreateSystemDefaultDevice()->retain();

	m_pSwapchain = (CA::MetalLayer*)sp;
	m_pSwapchain->setDevice(m_pDevice);
	m_pDrawable = m_pSwapchain->nextDrawable();

	CGSize sz = { (float)width, (float)height };
	m_pSwapchain->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
	m_pSwapchain->setDrawableSize(sz);

	LOG_INFO(std::string(m_pDevice->name()->cString(NS::ASCIIStringEncoding)));

	m_pCommandQueue = m_pDevice->newCommandQueue();
	BuildShaders();
	BuildBuffers();

	m_pRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
	m_pColorAttachment = m_pRenderPassDescriptor->colorAttachments()->object(0);
	m_pColorAttachment->setLoadAction(MTL::LoadAction::LoadActionClear);
	m_pColorAttachment->setStoreAction(MTL::StoreAction::StoreActionStore);
}

Renderer2D_Metal::~Renderer2D_Metal()
{
	m_pCommandQueue->release();
	m_pSwapchain->release();
	m_pDevice->release();
}

void Renderer2D_Metal::BuildShaders()
{
	NS::Error* pError = nullptr;
	MTL::Library* pLibrary = m_pDevice->newLibrary(NS::String::string(shaderSrc, NS::StringEncoding::UTF8StringEncoding), nullptr, &pError);
	if (!pLibrary)
	{
		LOG_ERROR(pError->localizedDescription()->utf8String());
		assert(false);
	}

	MTL::Function* pVertexFn = pLibrary->newFunction(NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding));
	MTL::Function* pFragmentFn = pLibrary->newFunction(NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding));

	MTL::VertexDescriptor* pDescriptor = MTL::VertexDescriptor::alloc()->init();
	pDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat2);
	pDescriptor->attributes()->object(0)->setBufferIndex(0);
	pDescriptor->attributes()->object(0)->setOffset(offsetof(Vertex, position));

	pDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
	pDescriptor->attributes()->object(1)->setBufferIndex(0);
	pDescriptor->attributes()->object(1)->setOffset(offsetof(Vertex, color));

	pDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));

	MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
	pDesc->setVertexDescriptor(pDescriptor);
	pDesc->setVertexFunction(pVertexFn);
	pDesc->setFragmentFunction(pFragmentFn);

	pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

	m_pRenderPipelineState = m_pDevice->newRenderPipelineState(pDesc, &pError);
	if (!m_pRenderPipelineState)
	{
		LOG_ERROR(pError->localizedDescription()->utf8String());
		assert(false);
	}

	pVertexFn->release();
	pFragmentFn->release();
	pDescriptor->release();
	pDesc->release();
	pLibrary->release();
}

void Renderer2D_Metal::BuildBuffers()
{
	const Vertex vertices[] = {
		{ { 0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	const unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};

	m_pVertexBuffer = m_pDevice->newBuffer(sizeof(vertices), MTL::ResourceStorageModeManaged);
	memcpy(m_pVertexBuffer->contents(), vertices, sizeof(vertices));
	m_pVertexBuffer->didModifyRange(NS::Range::Make(0, m_pVertexBuffer->length()));

	m_pIndexBuffer = m_pDevice->newBuffer(sizeof(indicies), MTL::ResourceStorageModeManaged);
	memcpy(m_pIndexBuffer->contents(), indicies, sizeof(indicies));
	m_pIndexBuffer->didModifyRange(NS::Range::Make(0, m_pIndexBuffer->length()));

}

void Renderer2D_Metal::Begin()
{
	m_pDrawable = m_pSwapchain->nextDrawable();

	m_pColorAttachment->setTexture(m_pDrawable->texture());

	// m_pAutoReleasePool = NS::AutoreleasePool::alloc()->init();

	m_pCmd = m_pCommandQueue->commandBuffer();
	m_pEnc = m_pCmd->renderCommandEncoder(m_pRenderPassDescriptor);
	m_pEnc->setRenderPipelineState(m_pRenderPipelineState);
}

void Renderer2D_Metal::End()
{
	m_pEnc->endEncoding();
	m_pCmd->presentDrawable(m_pDrawable);
	m_pCmd->commit();
}

void Renderer2D_Metal::Clear(const Color& color) const
{
	m_pColorAttachment->setClearColor( MTL::ClearColor::Make(color.r, color.g, color.b, color.a));
}

void Renderer2D_Metal::DrawDisk(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Metal::DrawCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Metal::FillCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Metal::DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_Metal::FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_Metal::DrawLine(const pt2di& start, const pt2di& end, const Color& color) const
{

}

void Renderer2D_Metal::DrawText(const pt2di& position, const std::string& text, const Color& color) const
{

}

void Renderer2D_Metal::DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const
{

}

void Renderer2D_Metal::DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const
{

}


Renderer2D::SpriteID Renderer2D_Metal::LoadTexture(const char* path, const std::string& tag)
{
	Texture texture = ::LoadTextureToGPU(path);
	m_textures.push_back(texture);
	size_t textureId = m_textures.size() - 1;
	m_sprites.push_back({ textureId, { 0.0f, 0.0f }, { 1.0f, 1.0f } });
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(tag, spriteId);

	return spriteId;
}

std::vector<Renderer2D::SpriteID> Renderer2D_Metal::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	Texture texture = ::LoadTextureToGPU(path);
	m_textures.push_back(texture);
	size_t textureId = m_textures.size() - 1;
	std::vector<SpriteID> sprites;

	float textureWidth = (float)texture.dim.w;
	float textureHeight = (float)texture.dim.h;

	for (const auto& desc : spriteDescriptors)
	{
		Sprite sprite;
		sprite.textureId = textureId;

		pt2di topLeft = desc.offset;
		switch (desc.offsetType)
		{
		case SpriteOffsetType::TopLeft:
			break;

		case SpriteOffsetType::TopRight:
			topLeft.x -= desc.size.w;
			break;

		case SpriteOffsetType::BottomLeft:
			topLeft.y -= desc.size.h;
			break;

		case SpriteOffsetType::BottomRight:
			topLeft = desc.offset - desc.size;
			break;
		
		default:
			LOG_ERROR("unknown SpriteOffsetType.");
			continue;
		}

		pt2di bottomRight = topLeft + desc.size;

		sprite.topLeft = { (float)topLeft.x / textureWidth, (float)topLeft.y / textureHeight };
		sprite.bottomRight = { (float)bottomRight.x / textureWidth, (float)bottomRight.y / textureHeight };

		m_sprites.push_back(sprite);
		SpriteID spriteId = m_sprites.size() - 1;
		m_tagsMap.emplace(desc.tag, spriteId);
	}

	return sprites;
}

bool Renderer2D_Metal::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const
{
	if (!IsValidSpriteId(spriteId))
		return false;

	// const auto normalizeCoords = [] (const pt2di& coord, const vec2di& viewPortSize) -> pt2df
	// {
	// 	return { 2.0f * (float)coord.x / (float)viewPortSize.w - 1.0f, -2.0f * (float)coord.y / (float)viewPortSize.h + 1.0f };
	// };

	// const auto normalizeSize = [] (const vec2di& sz, const vec2di& viewPortSize) -> vec2df
	// {
	// 	return { 2.0f * (float)sz.w / (float)viewPortSize.w, 2.0f * (float)sz.h / (float)viewPortSize.h };
	// };

	Sprite sprite = m_sprites[spriteId];
	Texture texture = m_textures[sprite.textureId];

	// pt2df normalizedPosition = normalizeCoords(topLeft, m_viewPortDim);
	// vec2df normalizedSize = normalizeSize(dimensions, m_viewPortDim);

	// TODO

	for (int i = 0; i < N_QUAD_VERTICES; ++i)
	{
		pt2df textureCoord = {  sprite.topLeft.x * quadWeights[i][0] + sprite.bottomRight.x * quadWeights[i][2],
								sprite.topLeft.y * quadWeights[i][1] + sprite.bottomRight.y * quadWeights[i][3] };
	}

	// send the texture as uniform
	// send the texture coords as uniform

	return true;
}

bool Renderer2D_Metal::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const
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


vec2di Renderer2D_Metal::GetSpriteSize(const std::string& spriteTag) const
{
	return {};
}

const vec2di& Renderer2D_Metal::GetCellDim() const
{
	return m_cellDim;
}

const vec2di& Renderer2D_Metal::GetViewPortDim() const
{
	return m_viewPortDim;
}

void Renderer2D_Metal::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}

void Renderer2D_Metal::SetViewPortDim(const vec2di& dim)
{
	// TODO recalculate projection matrix
	// TODO set the view port
	m_viewPortDim = dim;
	m_viewPort = (CGRect){ { 0.0, 0.0 }, { (double)dim.w, (double)dim.h } };
	// TODO
}
#endif