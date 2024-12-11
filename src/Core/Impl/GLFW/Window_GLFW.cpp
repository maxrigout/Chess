#include "Core/PlatformSupport.h"
#include "Window_GLFW.h"

#ifdef SUPPORT_OPENGL
#include "Renderer2D/OpenGL/Renderer2D_OpenGLFactory.h"
#endif

#ifdef SUPPORT_METAL

#include "Renderer2D/Metal/Renderer2D_Metal.h"
#endif

#ifdef SUPPORT_VULKAN
#define GLFW_INCLUDE_VULKAN
#include "Renderer2D/Vulkan/Renderer2D_Vulkan.h"
#endif

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "Core/Logger.h"

static MouseButton mouseButtonMap[6];
static Key keyboardMap[GLFW_KEY_LAST];
static bool isGLFWInitialized = false;
static int numInstances = 0;

Window_GLFW::Window_GLFW()
{
	mouseButtonMap[GLFW_MOUSE_BUTTON_LEFT] = MouseButton::Left;
	mouseButtonMap[GLFW_MOUSE_BUTTON_MIDDLE] = MouseButton::Middle;
	mouseButtonMap[GLFW_MOUSE_BUTTON_RIGHT] = MouseButton::Right;
	mouseButtonMap[GLFW_MOUSE_BUTTON_4] = MouseButton::X1;
	mouseButtonMap[GLFW_MOUSE_BUTTON_4] = MouseButton::X2;

	for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
		keyboardMap[i] = Key::Unknown;
	}

	keyboardMap[GLFW_KEY_A] = Key::A;
	keyboardMap[GLFW_KEY_B] = Key::B;
	keyboardMap[GLFW_KEY_C] = Key::C;
	keyboardMap[GLFW_KEY_D] = Key::D;
	keyboardMap[GLFW_KEY_E] = Key::E;
	keyboardMap[GLFW_KEY_F] = Key::F;
	keyboardMap[GLFW_KEY_G] = Key::G;
	keyboardMap[GLFW_KEY_H] = Key::H;
	keyboardMap[GLFW_KEY_I] = Key::I;
	keyboardMap[GLFW_KEY_J] = Key::J;
	keyboardMap[GLFW_KEY_K] = Key::K;
	keyboardMap[GLFW_KEY_L] = Key::L;
	keyboardMap[GLFW_KEY_M] = Key::M;
	keyboardMap[GLFW_KEY_N] = Key::N;
	keyboardMap[GLFW_KEY_O] = Key::O;
	keyboardMap[GLFW_KEY_P] = Key::P;
	keyboardMap[GLFW_KEY_Q] = Key::Q;
	keyboardMap[GLFW_KEY_R] = Key::R;
	keyboardMap[GLFW_KEY_S] = Key::S;
	keyboardMap[GLFW_KEY_T] = Key::T;
	keyboardMap[GLFW_KEY_U] = Key::U;
	keyboardMap[GLFW_KEY_V] = Key::V;
	keyboardMap[GLFW_KEY_W] = Key::W;
	keyboardMap[GLFW_KEY_X] = Key::X;
	keyboardMap[GLFW_KEY_Y] = Key::Y;
	keyboardMap[GLFW_KEY_Z] = Key::Z;

	keyboardMap[GLFW_KEY_0] = Key::K0;
	keyboardMap[GLFW_KEY_1] = Key::K1;
	keyboardMap[GLFW_KEY_2] = Key::K2;
	keyboardMap[GLFW_KEY_3] = Key::K3;
	keyboardMap[GLFW_KEY_4] = Key::K4;
	keyboardMap[GLFW_KEY_5] = Key::K5;
	keyboardMap[GLFW_KEY_6] = Key::K6;
	keyboardMap[GLFW_KEY_7] = Key::K7;
	keyboardMap[GLFW_KEY_8] = Key::K8;
	keyboardMap[GLFW_KEY_9] = Key::K9;

	keyboardMap[GLFW_KEY_SPACE]		= Key::Space;
	keyboardMap[GLFW_KEY_ENTER]		= Key::Enter;
	keyboardMap[GLFW_KEY_BACKSPACE] = Key::Backspace;
	keyboardMap[GLFW_KEY_TAB]		= Key::Tab;
	keyboardMap[GLFW_KEY_ESCAPE]	= Key::Escape;

	keyboardMap[GLFW_KEY_UP]	= Key::Up;
	keyboardMap[GLFW_KEY_DOWN]	= Key::Down;
	keyboardMap[GLFW_KEY_LEFT]	= Key::Left;
	keyboardMap[GLFW_KEY_RIGHT]	= Key::Right;

	keyboardMap[GLFW_KEY_F1]  = Key::F1;
	keyboardMap[GLFW_KEY_F2]  = Key::F2;
	keyboardMap[GLFW_KEY_F3]  = Key::F3;
	keyboardMap[GLFW_KEY_F4]  = Key::F4;
	keyboardMap[GLFW_KEY_F5]  = Key::F5;
	keyboardMap[GLFW_KEY_F6]  = Key::F6;
	keyboardMap[GLFW_KEY_F7]  = Key::F7;
	keyboardMap[GLFW_KEY_F8]  = Key::F8;
	keyboardMap[GLFW_KEY_F9]  = Key::F9;
	keyboardMap[GLFW_KEY_F10] = Key::F10;
	keyboardMap[GLFW_KEY_F11] = Key::F11;
	keyboardMap[GLFW_KEY_F12] = Key::F12;

	m_onWindowClose = defaultEventHandlerFunction<WindowCloseEvent>();
	m_onMouseMove = defaultEventHandlerFunction<MouseMoveEvent>();
	m_onMouseButtonDown = defaultEventHandlerFunction<MouseButtonDownEvent>();
	m_onMouseButtonUp = defaultEventHandlerFunction<MouseButtonUpEvent>();
	m_onMouseWheel = defaultEventHandlerFunction<MouseWheelEvent>();
	m_onKeyboardDown = defaultEventHandlerFunction<KeyboardDownEvent>();
	m_onKeyboardUp = defaultEventHandlerFunction<KeyboardUpEvent>();


	if (!isGLFWInitialized)
	{
		glfwInit();
		isGLFWInitialized = true;
	}

	numInstances++;
}

Window_GLFW::~Window_GLFW()
{
	glfwDestroyWindow(m_pWindow);

	numInstances--;
	if (numInstances <= 0)
	{
		glfwTerminate();
		isGLFWInitialized = false;
	}
}


void Window_GLFW::Create(const WindowCreationInfo& createInfo)
{
	m_rendererType = createInfo.flags.rendererBackend;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
	m_width = createInfo.width;
	m_height = createInfo.height;
	m_pWindow = glfwCreateWindow(m_width, m_height, createInfo.title.c_str(), nullptr, nullptr);
	if (m_pWindow == nullptr)
	{
		LOG_ERROR("Failed to create GLFW window");
	}

	glfwSetWindowUserPointer(m_pWindow, this);

	glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) {
		auto ptr = (Window_GLFW*)glfwGetWindowUserPointer(window);
		ptr->m_onWindowClose({});
	});

	glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods) {
		auto ptr = (Window_GLFW*)glfwGetWindowUserPointer(window);
		switch (action)
		{
			case GLFW_PRESS:
				ptr->m_onMouseButtonDown({mouseButtonMap[button]});
				break;
			case GLFW_RELEASE:
				ptr->m_onMouseButtonUp({mouseButtonMap[button]});
				break;
		}
	});

	glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xPos, double yPos) {
		auto ptr = (Window_GLFW*)glfwGetWindowUserPointer(window);
		ptr->m_onMouseMove({(int)xPos, (int)yPos});
	});

	glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
		auto ptr = (Window_GLFW*)glfwGetWindowUserPointer(window);
		ptr->m_onMouseWheel({(int)xOffset, (int)yOffset});
	});

	glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto ptr = (Window_GLFW*)glfwGetWindowUserPointer(window);
		switch (action)
		{
			case GLFW_PRESS:
				ptr->m_onKeyboardDown({keyboardMap[key]});
				break;
			case GLFW_RELEASE:
				ptr->m_onKeyboardUp({keyboardMap[key]});
				break;
		}
	});
}

Renderer2D* Window_GLFW::CreateRenderer()
{
	switch (m_rendererType)
	{
		case RendererBackendType::Unspec:
		case RendererBackendType::OpenGL: return CreateOpenGLRenderer();
		case RendererBackendType::Vulkan: return CreateVulkanRenderer();
		case RendererBackendType::Metal: return CreateMetalRenderer();
		case RendererBackendType::SDL2:
		case RendererBackendType::Unknown: return nullptr;
	}
}

Renderer2D* Window_GLFW::CreateOpenGLRenderer()
{
#ifdef SUPPORT_OPENGL
	Renderer2D_OpenGL::LoadOpenGLLibrary(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	Renderer2D_OpenGL* openGLRenderer = Renderer2D_OpenGLFactory::CreateOpenGLRenderer();
	openGLRenderer->OnRenderEnd(
		[this]()
		{
			glfwSwapBuffers(this->m_pWindow);
		});
	openGLRenderer->SetViewPortDim({ m_width, m_height });

	m_pRenderer = openGLRenderer;
#else
	throw std::runtime_error("OpenGL renderer is not supported!");
#endif
	return m_pRenderer;
}

Renderer2D* Window_GLFW::CreateVulkanRenderer()
{
#ifdef SUPPORT_VULKAN
	auto instanceExtensionGetter = [this](unsigned int* extCount, const char** exts) -> bool
	{
		// TODO: need to refactor the extension getter
		// return SDL_Vulkan_GetInstanceExtensions(this->m_pWindow, extCount, exts) == SDL_TRUE;
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		if (exts != nullptr)
		{
			for (int i = 0; i < glfwExtensionCount; ++i)
			{
				exts[i] = glfwExtensions[i];
			}
		}
		*extCount = glfwExtensionCount;
		return true;
	};

	auto surfaceCreator = [this](VkInstance instance, VkSurfaceKHR* surface) -> bool
	{
		if (glfwCreateWindowSurface(instance, this->m_pWindow, nullptr, surface) == VK_SUCCESS)
		{
			return true;
		}
		const char* errorMsg;
		glfwGetError(&errorMsg);
		LOG_ERROR(errorMsg);
		return false;
	};
	
	auto* vulkanRenderer = new Renderer2D_Vulkan();
	vulkanRenderer->SetInstanceExtensionProvider(/* TODO */);
	vulkanRenderer->SetSurfaceProvider(/* TODO */);
	vulkanRenderer->SetInstanceExtensionGetter(instanceExtensionGetter);
	vulkanRenderer->SetSurfaceCreator(surfaceCreator);
	vulkanRenderer->SetViewPortDim({ m_width, m_height });
	vulkanRenderer->Init();
	
	m_pRenderer = vulkanRenderer;
#else
	throw std::runtime_error("Vulkan renderer is not supported!");
#endif
	return m_pRenderer;
}

Renderer2D* Window_GLFW::CreateMetalRenderer()
{
#ifdef SUPPORT_METAL
	// SDL_MetalView view = SDL_Metal_CreateView(m_pWindow);
	// m_onDeleteRenderer = [=]() {
	// 	// SDL_Metal_DestroyView(view);
	// };
	// Renderer2D_Metal* pRenderer = new Renderer2D_Metal(SDL_Metal_GetLayer(view), m_width, m_height);
	// m_pRenderer = pRenderer;
	// m_pRenderer->SetViewPortDim({ m_width, m_height });
#else
	throw std::runtime_error("Metal renderer is not supported!");
#endif
	return m_pRenderer;
}


void Window_GLFW::FreeRenderer()
{
	delete m_pRenderer;
}


void Window_GLFW::PollEvents()
{
	glfwPollEvents();
	// if (glfwWindowShouldClose(m_pWindow))
	// {
	// 	m_onWindowClose({});
	// }
}

int Window_GLFW::GetWidth() const
{
	return m_width;
}

int Window_GLFW::GetHeight() const
{
	return m_height;
}

void Window_GLFW::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_pWindow, title.c_str());
}

void Window_GLFW::OnWindowClose(WindowCloseCallback&& callback)
{
	m_onWindowClose = callback;
}

void Window_GLFW::OnMouseMove(MouseMoveCallback&& callback)
{
	m_onMouseMove = callback;
}

void Window_GLFW::OnMouseButtonDown(MouseButtonDownCallback&& callback)
{
	m_onMouseButtonDown = callback;
}

void Window_GLFW::OnMouseButtonUp(MouseButtonUpCallback&& callback)
{
	m_onMouseButtonUp = callback;
}

void Window_GLFW::OnMouseScrolled(MouseWheelCallback&& callback)
{
	m_onMouseWheel = callback;
}

void Window_GLFW::OnKeyboardDown(KeyboardDownCallback&& callback)
{
	m_onKeyboardDown = callback;
}

void Window_GLFW::OnKeyboardUp(KeyboardUpCallback&& callback)
{
	m_onKeyboardUp = callback;
}
