#include "Renderer2D_Metal.h"
#ifdef SUPPORT_METAL
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#include "Core/Logger.h"

#include <iostream>
#include <string>

#include <stb/stb_image.h>

#define PRIMITIVE_RESTART_INDEX 0xFFFFFFFF
#define MAX_FRAMES_IN_FLIGHT 1

static const char* shaderSrc = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
	float4 position [[position]];
	half4 color;
	float2 texCoord;
};
struct VertexIn
{
	float2 position [[attribute(0)]];
	float4 color [[attribute(1)]];
	float2 textureCoords [[attribute(2)]];
};
struct UniformBuffer
{
	float2 offset;
	float2 scale;
	float2 textureCoords[4];
};

VertexOut vertex vertexMain(VertexIn v [[stage_in]],
							uint vertexId [[vertex_id]],
							uint instanceId [[instance_id]],
							constant UniformBuffer& ub [[buffer(1)]])
{
	float2 pos = v.position * ub.scale + ub.offset;
	VertexOut out;
	out.position = float4(pos, 1.0, 1.0);
	out.color = half4(v.color);
	out.texCoord = ub.textureCoords[vertexId];
	return out;
}

half4 fragment fragmentMain(VertexOut in [[stage_in]], texture2d<half, access::sample> tex [[texture(0)]])
{
	constexpr sampler s(address::repeat, filter::linear);
	return tex.sample(s, in.texCoord);
}
)";

// static const int maxTexturesBind = 4;

/*
	0 ------ 3
	|        |
	|        |
	1 ------ 2
*/

// https://metalbyexample.com/modern-metal-1/
// https://metalbyexample.com/modern-metal-2/

// https://github.com/gzorin/sdl-metal-cpp-example/blob/main/main.cpp
// https://gist.github.com/gcatlin/b89e0efed78dd91364609ca4095da346
// https://schneide.blog/2022/03/28/metal-in-c-with-sdl2/

/* TO DEBUG METAL
	export NSZombieEnabled=YES
	export OBJC_DEBUG_MISSING_POOLS=YES
*/

static const std::vector<Renderer2D_Metal::Vertex> quadVertices = {
	{ { 0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
	{ { 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { 1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const std::vector<uint16_t> quadIndices {
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

struct UniformBuffer
{
	simd::float2 offset;
	simd::float2 scale;
	simd::float2 textureCoords[N_QUAD_VERTICES];
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

static Renderer2D_Metal::Texture LoadTextureToGPU(MTL::Device* pDevice, const char* path)
{
	const size_t maxMsgSz = 500;
	char message[maxMsgSz] = { 0 };
	int width, height, nComponents;
	unsigned char* imageData = stbi_load(path, &width, &height, &nComponents, STBI_rgb_alpha);
	if (!imageData)
	{
		// cannot load image...
		snprintf(message, maxMsgSz, "unable to load %s", path);
		stbi_image_free(imageData);
		return {{}, {-1, -1}, path};
	}

	MTL::TextureDescriptor* pTexDesc = MTL::TextureDescriptor::alloc()->init();
	pTexDesc->setWidth(width);
	pTexDesc->setHeight(height);
	pTexDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
	pTexDesc->setTextureType(MTL::TextureType2D);
	pTexDesc->setStorageMode(MTL::StorageModeManaged);
	pTexDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

	auto pTexture = NS::TransferPtr(pDevice->newTexture(pTexDesc));
	pTexture->replaceRegion(MTL::Region(0, 0, 0, width, height, 1), 0, imageData, width * 4);

	stbi_image_free(imageData);

	Renderer2D_Metal::Texture texture;
	texture.texture = pTexture;
	texture.dim = { width, height };
	texture.path = path;

	snprintf(message, maxMsgSz, "loaded %s", path);

	LOG_DEBUG(message);

	return texture;
}

Renderer2D_Metal::Renderer2D_Metal(void* sp, int width, int height)
{
	m_pGlobalAutoreleasePool = NS::AutoreleasePool::alloc()->init();

	m_pDevice = MTL::CreateSystemDefaultDevice()->retain();

	// CGRect frame = (CGRect){ {100.0, 100.0}, {(float)width, (float)height} };
	// m_pView = MTK::View::alloc()->init( frame, m_pDevice );
	// m_pView->setColorPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );

	CGSize sz = { (float)width, (float)height };

	m_pSwapchain = (CA::MetalLayer*)sp;
	m_pSwapchain->setDevice(m_pDevice);
	m_pSwapchain->setDisplaySyncEnabled(true);
	m_pSwapchain->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
	m_pSwapchain->setDrawableSize(sz);

	LOG_INFO(std::string(m_pDevice->name()->cString(NS::ASCIIStringEncoding)));

	m_pCommandQueue = m_pDevice->newCommandQueue();
	BuildShaders();
	BuildBuffers();

	m_semaphore = dispatch_semaphore_create(MAX_FRAMES_IN_FLIGHT);

	m_pRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
	m_pColorAttachment = m_pRenderPassDescriptor->colorAttachments()->object(0);
	m_pColorAttachment->setLoadAction(MTL::LoadAction::LoadActionClear);
	m_pColorAttachment->setStoreAction(MTL::StoreAction::StoreActionStore);
}

Renderer2D_Metal::~Renderer2D_Metal()
{
	dispatch_semaphore_wait(m_semaphore, DISPATCH_TIME_FOREVER);
	m_pCommandQueue->release();
	m_pSwapchain->release();
	m_pDevice->release();
	m_pGlobalAutoreleasePool->release();
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

	pDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormat::VertexFormatFloat2);
	pDescriptor->attributes()->object(2)->setBufferIndex(0);
	pDescriptor->attributes()->object(2)->setOffset(offsetof(Vertex, textureCoords));

	pDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));

	MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
	pDesc->setVertexDescriptor(pDescriptor);
	pDesc->setVertexFunction(pVertexFn);
	pDesc->setFragmentFunction(pFragmentFn);

	pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
	pDesc->colorAttachments()->object(0)->setBlendingEnabled(true);
	pDesc->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperationAdd);
	pDesc->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
	pDesc->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorZero);
	pDesc->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperationAdd);
	pDesc->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
	pDesc->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);

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
	m_pVertexBuffer = m_pDevice->newBuffer(sizeof(quadVertices), MTL::ResourceStorageModeManaged);
	memcpy(m_pVertexBuffer->contents(), quadVertices.data(), quadVertices.size() * sizeof(Vertex));
	m_pVertexBuffer->didModifyRange(NS::Range::Make(0, m_pVertexBuffer->length()));

	m_pIndexBuffer = m_pDevice->newBuffer(sizeof(quadIndices), MTL::ResourceStorageModeManaged);
	memcpy(m_pIndexBuffer->contents(), quadIndices.data(), quadIndices.size() * sizeof(uint16_t));
	m_pIndexBuffer->didModifyRange(NS::Range::Make(0, m_pIndexBuffer->length()));

	// m_pUniformBuffer = m_pDevice->newBuffer(sizeof(UniformBuffer), MTL::ResourceStorageModeManaged);
}

void Renderer2D_Metal::Begin()
{
	m_pAutoReleasePool = NS::AutoreleasePool::alloc()->init();

	dispatch_semaphore_wait(m_semaphore, DISPATCH_TIME_FOREVER);

	m_pDrawable = m_pSwapchain->nextDrawable();
	m_pColorAttachment->setTexture(m_pDrawable->texture());
	m_pCmd = m_pCommandQueue->commandBuffer();

	m_pCmd->addCompletedHandler([this](MTL::CommandBuffer* pCmd){
        dispatch_semaphore_signal(m_semaphore);
    });

	m_pEnc = m_pCmd->renderCommandEncoder(m_pRenderPassDescriptor);
	m_pEnc->setRenderPipelineState(m_pRenderPipelineState);
}

void Renderer2D_Metal::End()
{
	m_pEnc->endEncoding();
	m_pCmd->presentDrawable(m_pDrawable);
	m_pCmd->commit();

	// m_pDrawable->present();

	m_pAutoReleasePool->release();
}

void Renderer2D_Metal::Clear(const Color& color) const
{
	MTL::ClearColor clearColor = MTL::ClearColor::Make(color.r, color.g, color.b, color.a);
	m_pColorAttachment->setClearColor(clearColor);
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
	Texture texture = ::LoadTextureToGPU(m_pDevice, path);
	m_textures.emplace(path, texture);
	std::string textureId = path;
	m_sprites.push_back({ textureId, { 0.0f, 0.0f }, { 1.0f, 1.0f } });
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(tag, spriteId);

	return spriteId;
}

Renderer2D_Metal::Texture& Renderer2D_Metal::GetTexture(const std::string& path)
{
	if (m_textures.count(path) == 0)
	{
		Texture t = ::LoadTextureToGPU(m_pDevice, path.c_str());
		m_textures.emplace(path, t);
	}
	return m_textures[path];
}

Renderer2D::SpriteID Renderer2D_Metal::LoadSprite(const SpriteDescriptor& spriteDescriptor) 
{
	Texture texture = GetTexture(spriteDescriptor.texturePath.c_str());

	float textureWidth = (float)texture.dim.w;
	float textureHeight = (float)texture.dim.h;

	Sprite sprite;
	sprite.textureId = texture.path;

	pt2di topLeft = spriteDescriptor.offset;
	vec2di effectiveSize = spriteDescriptor.size;
	if (spriteDescriptor.size.x == -1)
	{
		effectiveSize.w = texture.dim.w;
	}

	if (spriteDescriptor.size.y == -1)
	{
		effectiveSize.h = texture.dim.h;
	}

	switch (spriteDescriptor.offsetType)
	{
	case SpriteOffsetType::TopLeft:
		break;

	case SpriteOffsetType::TopRight:
		topLeft.x -= effectiveSize.w;
		break;

	case SpriteOffsetType::BottomLeft:
		topLeft.y -= effectiveSize.h;
		break;

	case SpriteOffsetType::BottomRight:
		topLeft = spriteDescriptor.offset - effectiveSize;
		break;
	
	default:
		LOG_ERROR("unknown SpriteOffsetType.");
		return -1;
	}

	pt2di bottomRight = topLeft + effectiveSize;

	sprite.topLeft = { (float)topLeft.x / textureWidth, (float)topLeft.y / textureHeight };
	sprite.bottomRight = { (float)bottomRight.x / textureWidth, (float)bottomRight.y / textureHeight };

	m_sprites.push_back(sprite);
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(spriteDescriptor.tag, spriteId);
	return spriteId;
}

std::vector<Renderer2D::SpriteID> Renderer2D_Metal::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	std::vector<SpriteID> sprites;

	for (const auto& desc : spriteDescriptors)
	{
		SpriteID spriteId = LoadSprite(desc);
		if (spriteId == -1) {
			continue;
		}
		m_tagsMap.emplace(desc.tag, spriteId);
	}

	return sprites;
}

bool Renderer2D_Metal::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const
{
	if (!IsValidSpriteId(spriteId))
		return false;

	const auto normalizeCoords = [] (const pt2di& coord, const vec2di& viewPortSize) -> simd::float2
	{
		return { 2.0f * (float)coord.x / (float)viewPortSize.w - 1.0f, -2.0f * (float)coord.y / (float)viewPortSize.h + 1.0f };
	};

	const auto normalizeSize = [] (const vec2di& sz, const vec2di& viewPortSize) -> simd::float2
	{
		return { 2.0f * (float)sz.w / (float)viewPortSize.w, 2.0f * (float)sz.h / (float)viewPortSize.h };
	};

	Sprite sprite = m_sprites[spriteId];
	Texture texture = m_textures.at(sprite.textureId);

	UniformBuffer uniform;

	for (int i = 0; i < N_QUAD_VERTICES; ++i)
	{
		uniform.textureCoords[i] = {  sprite.topLeft.x * quadWeights[i][0] + sprite.bottomRight.x * quadWeights[i][2],
								sprite.topLeft.y * quadWeights[i][1] + sprite.bottomRight.y * quadWeights[i][3] };
	}
	uniform.offset = normalizeCoords(topLeft, m_viewPortDim);
	uniform.scale = normalizeSize(dimensions, m_viewPortDim);

	// Draw
	m_pEnc->setVertexBuffer(m_pVertexBuffer, 0, 0);
	// m_pEnc->setVertexBuffer(m_pUniformBuffer, 0, 1);
	m_pEnc->setVertexBytes(&uniform, sizeof(UniformBuffer), 1);
	m_pEnc->setFragmentTexture(texture.texture.get(), 0);
	m_pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 6, MTL::IndexType::IndexTypeUInt16, m_pIndexBuffer, NS::Integer(0));

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
		LOG_ERROR("an error has occured with tag: ", tag, " - ", e.what());
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
	CGSize sz = { (float)dim.w, (float)dim.h };
	m_viewPort = CGRect{ { 0.0, 0.0 }, sz };
	m_pSwapchain->setDrawableSize(sz);
}
#endif