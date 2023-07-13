#pragma once
#include "Core/Window.h"

#include <SDL2/SDL.h>

class Window_SDL : public Window
{
public:
	Window_SDL();
	virtual ~Window_SDL();

	virtual void Create(const WindowCreationInfo& createInfo) override;
	virtual Renderer2D* CreateRenderer() override;
	virtual void FreeRenderer() override;

	virtual void PollEvents() override;
	virtual int GetWidth() const override;
	virtual int GetHeight() const override;
	virtual void SetTitle(const std::string& title) override;

	virtual void OnWindowClose(WindowCloseCallback&& callback) override;
	virtual void OnMouseMove(MouseMoveCallback&& callback) override;
	virtual void OnMouseButtonDown(MouseButtonDownCallback&& callback) override;
	virtual void OnMouseButtonUp(MouseButtonUpCallback&& callback) override;
	virtual void OnMouseScrolled(MouseWheelCallback&& callback) override;
	virtual void OnKeyboardDown(KeyboardDownCallback&& callback) override;
	virtual void OnKeyboardUp(KeyboardUpCallback&& callback) override;

private:

	Renderer2D* CreateSDLRenderer();
	Renderer2D* CreateOpenGLRenderer();
	Renderer2D* CreateVulkanRenderer();
	Renderer2D* CreateMetalRenderer();
	Renderer2D* CreateDirectXRenderer();

	RendererBackendType m_rendererType;
	Renderer2D* m_pRenderer = nullptr;

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pSDLRenderer = nullptr;
	int m_width;
	int m_height;

	WindowCloseCallback m_onWindowClose;
	MouseMoveCallback m_onMouseMove;
	MouseButtonDownCallback m_onMouseButtonDown;
	MouseButtonUpCallback m_onMouseButtonUp;
	MouseWheelCallback m_onMouseWheel;
	KeyboardDownCallback m_onKeyboardDown;
	KeyboardUpCallback m_onKeyboardUp;

	std::function<void(void)> m_onDeleteRenderer = [](){};
};
