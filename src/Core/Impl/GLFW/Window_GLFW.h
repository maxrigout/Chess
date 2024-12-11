#pragma once

#include "Core/Window.h"

struct GLFWwindow;

class Window_GLFW : public Window
{
public:
	Window_GLFW();
	virtual ~Window_GLFW();

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
	Renderer2D* CreateOpenGLRenderer();
	Renderer2D* CreateVulkanRenderer();
	Renderer2D* CreateMetalRenderer();

	RendererBackendType m_rendererType;

	GLFWwindow* m_pWindow = nullptr;
	Renderer2D* m_pRenderer = nullptr;

	int m_width;
	int m_height;

	WindowCloseCallback m_onWindowClose;
	MouseMoveCallback m_onMouseMove;
	MouseButtonDownCallback m_onMouseButtonDown;
	MouseButtonUpCallback m_onMouseButtonUp;
	MouseWheelCallback m_onMouseWheel;
	KeyboardDownCallback m_onKeyboardDown;
	KeyboardUpCallback m_onKeyboardUp;

};