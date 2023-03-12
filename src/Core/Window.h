#pragma once
#include "PlatformConfig.h"
#include "Input.h"
#include "Renderer2D/Renderer2D.h"

#include <functional>

struct RendererContext
{
	union
	{
		struct OpenGLContext
		{
			int versionMajor = 4;
			int versionMinor = 2;
			bool doubleBuffer = true;
			
		} opengl;

		struct VulkanContext
		{

		} vulkan;

		struct MetalContext
		{

		} metal;
	} context;
	RendererBackendType rendererBackend = RendererBackendType::Unspec;
};

struct WindowFlags
{
	RendererBackendType rendererBackend = RendererBackendType::Unspec;
};

struct WindowCreationInfo
{
	std::string title = "Window";
	int width = 1280;
	int height = 720;
	int x = -1;
	int y = -1;
	WindowFlags flags{};
	RendererContext rendererContext{};
};

// sdl2 metal https://gist.github.com/gcatlin/b89e0efed78dd91364609ca4095da346
// glfw metal https://gist.github.com/gcatlin/987be74e2d58da96093a7598f3fbfb27
class Window
{
public:
	using WindowCloseCallback		= std::function<bool(const WindowCloseEvent&)>;
	using MouseMoveCallback			= std::function<bool(const MouseMoveEvent&)>;
	using MouseButtonDownCallback	= std::function<bool(const MouseButtonDownEvent&)>;
	using MouseButtonUpCallback		= std::function<bool(const MouseButtonUpEvent&)>;
	using MouseWheelCallback		= std::function<bool(const MouseWheelEvent&)>;
	using KeyboardDownCallback		= std::function<bool(const KeyboardDownEvent&)>;
	using KeyboardUpCallback		= std::function<bool(const KeyboardUpEvent&)>;

	virtual ~Window() = default;

	virtual void Create(const WindowCreationInfo& createInfo) = 0;
	// TODO RendererConfig
	virtual Renderer2D* CreateRenderer() = 0;
	virtual void FreeRenderer() = 0;

	virtual void PollEvents() = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual void SetTitle(const std::string& title) = 0;

	virtual void OnWindowClose(WindowCloseCallback&& callback) = 0;
	virtual void OnMouseMove(MouseMoveCallback&& callback) = 0;
	virtual void OnMouseButtonDown(MouseButtonDownCallback&& callback) = 0;
	virtual void OnMouseButtonUp(MouseButtonUpCallback&& callback) = 0;
	virtual void OnMouseScrolled(MouseWheelCallback&& callback) = 0;
	virtual void OnKeyboardDown(KeyboardDownCallback&& callback) = 0;
	virtual void OnKeyboardUp(KeyboardUpCallback&& callback) = 0;
};
