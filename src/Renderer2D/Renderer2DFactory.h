#pragma once
#include "Renderer2D.h"
#include "Renderer2DCallbacks.h"
#include "RendererBackendType.h"

class Renderer2DFactory
{
public:
	Renderer2DFactory(
		RendererBackendType 				rendererBackend,
		OpenGLProcAddr_t					openglProcAddr,
		OpenGLOnRendererEnd_t				onRendererEnd,
		VulkanInstanceExtensionGetterFn_t	vulkanExentionGetter,
		VulkanSurfaceCreatorFn_t			vulkanCreateSurface);
	Renderer2D* CreateRenderer(int width, int height);

private:

	Renderer2D* CreateSDL2Renderer(int width, int height);
	Renderer2D* CreateOpenGLRenderer(int width, int height);
	Renderer2D* CreateMetalRenderer(int width, int height);
	Renderer2D* CreateVulkanRenderer(int width, int height);

	RendererBackendType 				m_rendererBackend;

	OpenGLProcAddr_t					m_openglProcAddr;
	OpenGLOnRendererEnd_t				m_onRendererEnd;
	VulkanInstanceExtensionGetterFn_t	m_vulkanExentionGetter;
	VulkanSurfaceCreatorFn_t			m_vulkanSurfaceCreator;
};