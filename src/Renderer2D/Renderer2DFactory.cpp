// #include "Core/PlatformSupport.h"
// #include "Renderer2DFactory.h"
// #include "Renderer2D.h"

// #ifdef SUPPORT_SDL
// #include "SDL/Renderer2D_SDL.h"
// #endif

// #ifdef SUPPORT_OPENGL
// #include "OpenGL/Renderer2D_OpenGL.h"
// #endif

// #ifdef SUPPORT_METAL
// #include "Metal/Renderer2D_Metal.h"
// #endif

// #ifdef SUPPORT_VULKAN
// #include "Vulkan/Renderer2D_Vulkan.h"
// #endif

// // need SDL2 Renderer
// // need opengl procaddress
// // need metal sqap chain?
// Renderer2DFactory::Renderer2DFactory(
// 	RendererBackendType					rendererBackend,
// 	OpenGLProcAddr_t					openglProcAddr,
// 	OpenGLOnRendererEnd_t				onRendererEnd,
// 	VulkanInstanceExtensionGetterFn_t	vulkanExentionGetter,
// 	VulkanSurfaceCreatorFn_t			vulkanCreateSurface,
// )
// {
// 	m_rendererBackend = rendererBackend;
// 	m_openglProcAddr = openglProcAddr;
// 	m_onRendererEnd = onRendererEnd;
// 	m_vulkanExentionGetter = vulkanExentionGetter;
// 	m_vulkanSurfaceCreator = vulkanCreateSurface;
// }

// Renderer2D* Renderer2DFactory::CreateRenderer(int width, int height)
// {
// 	switch (m_rendererBackend)
// 	{
// 	case RendererBackendType::SDL2: return CreateSDL2Renderer(width, height);
// 	case RendererBackendType::OpenGL: return CreateOpenGLRenderer(width, height);
// 	case RendererBackendType::Metal: return CreateMetalRenderer(width, height);
// 	case RendererBackendType::Vulkan: return CreateVulkanRenderer(width, height);
// 	}
// 	return nullptr;
// }

// Renderer2D* Renderer2DFactory::CreateSDL2Renderer(int width, int height)
// {
// 	Renderer2D_SDL* sdlRenderer = new Renderer2D_SDL(m_sdlRenderer);
// 	sdlRenderer->SetViewPortDim({ width, height });
// 	return sdlRenderer;
// }

// Renderer2D* Renderer2DFactory::CreateOpenGLRenderer(int width, int height)
// {
// 	Renderer2D_OpenGL::LoadOpenGLLibrary(m_openglProcAddr);
// 	Renderer2D_OpenGL* openglRenderer = new Renderer2D_OpenGL();
// 	openglRenderer->OnRenderEnd(m_onRendererEnd);
// 	openglRenderer->SetViewPortDim({ width, height });
// 	return openglRenderer;
// }

// Renderer2D* Renderer2DFactory::CreateMetalRenderer(int width, int height)
// {
// 	Renderer2D_Metal* metalRenderer = new Renderer2D_Metal(m_metalSwapchain);
// 	metalRenderer->SetViewPortDim({ width, height });
// 	return metalRenderer;
// }

// Renderer2D* Renderer2DFactory::CreateVulkanRenderer(int width, int height)
// {
// 	Renderer2D_Vulkan* vulkanRenderer = new Renderer2D_Vulkan();
// 	vulkanRenderer->SetInstanceExtensionGetter(m_vulkanExentionGetter);
// 	vulkanRenderer->SetSurfaceCreator(m_vulkanSurfaceCreator);
// 	vulkanRenderer->SetViewPortDim({ width, height });
// 	vulkanRenderer->Init();
// 	return vulkanRenderer;
// }
