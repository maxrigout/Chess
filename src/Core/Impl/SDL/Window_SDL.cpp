#include "Core/PlatformSupport.h"
#include "Window_SDL.h"

#include "Renderer2D/Renderer2DFactory.h"

#include "Renderer2D/SDL/Renderer2D_SDL.h"

#ifdef SUPPORT_OPENGL
#include "Renderer2D/OpenGL/Renderer2D_OpenGLFactory.h"
#endif

#ifdef SUPPORT_METAL
#include <SDL2/SDL_metal.h>
#include "Renderer2D/Metal/Renderer2D_Metal.h"
#endif

#ifdef SUPPORT_VULKAN
#include "Renderer2D/Vulkan/Renderer2D_Vulkan.h"
#include <SDL2/SDL_vulkan.h>
#endif

#include "Core/Logger.h"

// #include <SDL2/SDL_syswm.h>
#include <stdexcept>
#include <string>

static MouseButton mouseButtonMap[6];
static Key keyboardMap[SDL_NUM_SCANCODES];
static bool isSDLInitialized = false;
static int numInstances = 0;
static int isOpenGLInitialized = false;

static Uint32 GetWindowFlags(const WindowFlags& flags)
{
	Uint32 f = 0;
	switch (flags.rendererBackend)
	{
		case RendererBackendType::Vulkan:
			f |= SDL_WINDOW_VULKAN;
			break;
		case RendererBackendType::OpenGL:
			f |= SDL_WINDOW_OPENGL;
			break;
		case RendererBackendType::Metal:
			f |= SDL_WINDOW_METAL;
			break;
		case RendererBackendType::SDL2:
		case RendererBackendType::Unspec:
			f |= 0;
			break;
		default:
			throw std::runtime_error("backend type unsupported");
	}
	return f;
}

Window_SDL::Window_SDL()
{
	mouseButtonMap[SDL_BUTTON_LEFT] = MouseButton::Left;
	mouseButtonMap[SDL_BUTTON_MIDDLE] = MouseButton::Middle;
	mouseButtonMap[SDL_BUTTON_RIGHT] = MouseButton::Right;
	mouseButtonMap[SDL_BUTTON_X1] = MouseButton::X1;
	mouseButtonMap[SDL_BUTTON_X2] = MouseButton::X2;

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		keyboardMap[i] = Key::Unknown;
	}

	keyboardMap[SDL_SCANCODE_A] = Key::A;
	keyboardMap[SDL_SCANCODE_B] = Key::B;
	keyboardMap[SDL_SCANCODE_C] = Key::C;
	keyboardMap[SDL_SCANCODE_D] = Key::D;
	keyboardMap[SDL_SCANCODE_E] = Key::E;
	keyboardMap[SDL_SCANCODE_F] = Key::F;
	keyboardMap[SDL_SCANCODE_G] = Key::G;
	keyboardMap[SDL_SCANCODE_H] = Key::H;
	keyboardMap[SDL_SCANCODE_I] = Key::I;
	keyboardMap[SDL_SCANCODE_J] = Key::J;
	keyboardMap[SDL_SCANCODE_K] = Key::K;
	keyboardMap[SDL_SCANCODE_L] = Key::L;
	keyboardMap[SDL_SCANCODE_M] = Key::M;
	keyboardMap[SDL_SCANCODE_N] = Key::N;
	keyboardMap[SDL_SCANCODE_O] = Key::O;
	keyboardMap[SDL_SCANCODE_P] = Key::P;
	keyboardMap[SDL_SCANCODE_Q] = Key::Q;
	keyboardMap[SDL_SCANCODE_R] = Key::R;
	keyboardMap[SDL_SCANCODE_S] = Key::S;
	keyboardMap[SDL_SCANCODE_T] = Key::T;
	keyboardMap[SDL_SCANCODE_U] = Key::U;
	keyboardMap[SDL_SCANCODE_V] = Key::V;
	keyboardMap[SDL_SCANCODE_W] = Key::W;
	keyboardMap[SDL_SCANCODE_X] = Key::X;
	keyboardMap[SDL_SCANCODE_Y] = Key::Y;
	keyboardMap[SDL_SCANCODE_Z] = Key::Z;

	keyboardMap[SDL_SCANCODE_0] = Key::K0;
	keyboardMap[SDL_SCANCODE_1] = Key::K1;
	keyboardMap[SDL_SCANCODE_2] = Key::K2;
	keyboardMap[SDL_SCANCODE_3] = Key::K3;
	keyboardMap[SDL_SCANCODE_4] = Key::K4;
	keyboardMap[SDL_SCANCODE_5] = Key::K5;
	keyboardMap[SDL_SCANCODE_6] = Key::K6;
	keyboardMap[SDL_SCANCODE_7] = Key::K7;
	keyboardMap[SDL_SCANCODE_8] = Key::K8;
	keyboardMap[SDL_SCANCODE_9] = Key::K9;

	keyboardMap[SDL_SCANCODE_SPACE]		= Key::Space;
	keyboardMap[SDL_SCANCODE_RETURN]	= Key::Enter;
	keyboardMap[SDL_SCANCODE_BACKSPACE] = Key::Backspace;
	keyboardMap[SDL_SCANCODE_TAB]		= Key::Tab;
	keyboardMap[SDL_SCANCODE_ESCAPE]	= Key::Escape;

	keyboardMap[SDL_SCANCODE_UP]	= Key::Up;
	keyboardMap[SDL_SCANCODE_DOWN]	= Key::Down;
	keyboardMap[SDL_SCANCODE_LEFT]	= Key::Left;
	keyboardMap[SDL_SCANCODE_RIGHT]	= Key::Right;

	keyboardMap[SDL_SCANCODE_F1]  = Key::F1;
	keyboardMap[SDL_SCANCODE_F2]  = Key::F2;
	keyboardMap[SDL_SCANCODE_F3]  = Key::F3;
	keyboardMap[SDL_SCANCODE_F4]  = Key::F4;
	keyboardMap[SDL_SCANCODE_F5]  = Key::F5;
	keyboardMap[SDL_SCANCODE_F6]  = Key::F6;
	keyboardMap[SDL_SCANCODE_F7]  = Key::F7;
	keyboardMap[SDL_SCANCODE_F8]  = Key::F8;
	keyboardMap[SDL_SCANCODE_F9]  = Key::F9;
	keyboardMap[SDL_SCANCODE_F10] = Key::F10;
	keyboardMap[SDL_SCANCODE_F11] = Key::F11;
	keyboardMap[SDL_SCANCODE_F12] = Key::F12;

	m_onWindowClose = [](const WindowCloseEvent&) { return false; };
	m_onMouseMove = [](const MouseMoveEvent&) { return false; };
	m_onMouseButtonDown = [](const MouseButtonDownEvent&) { return false; };
	m_onMouseButtonUp = [](const MouseButtonUpEvent&) { return false; };
	m_onMouseWheel = [](const MouseWheelEvent&) { return false; };
	m_onKeyboardDown = [](const KeyboardDownEvent&) { return false; };
	m_onKeyboardUp = [](const KeyboardUpEvent&) { return false; };


	if (!isSDLInitialized)
	{
		SDL_Init(SDL_INIT_VIDEO);
		isSDLInitialized = true;
	}

	numInstances++;
}

Window_SDL::~Window_SDL()
{
	if (m_pRenderer != nullptr)
	{
		SDL_DestroyRenderer(m_pSDLRenderer);
	}
	
	SDL_DestroyWindow(m_pWindow);

	numInstances--;
	if (numInstances <= 0)
	{
		SDL_Quit();
		isSDLInitialized = false;
	}
}

void Window_SDL::Create(const WindowCreationInfo& createInfo)
{
	m_width = createInfo.width;
	m_height = createInfo.height;
	int x = createInfo.x;
	int y = createInfo.y;
	if (x < 0)
		x = SDL_WINDOWPOS_UNDEFINED;
	if (y < 0)
		y = SDL_WINDOWPOS_UNDEFINED;
	
	m_rendererType = createInfo.flags.rendererBackend;
	Uint32 windowFlags = GetWindowFlags(createInfo.flags);
	
#ifdef SUPPORT_OPENGL
	// need to load opengl before creating the window
	if (createInfo.flags.rendererBackend == RendererBackendType::OpenGL && !isOpenGLInitialized)
	{
		// Default OpenGL is fine.
		if (SDL_GL_LoadLibrary(NULL) != 0)
			LOG_ERROR(std::string("cannot load OpenGL library") + SDL_GetError());
		// Request an OpenGL 4.5 context (should be core)
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		// Also request a depth buffer
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		isOpenGLInitialized = true;
	}
#endif
	// SDL_SysWMinfo info;
	
	m_pWindow = SDL_CreateWindow(
		createInfo.title.c_str(), // window title
		x, // window x
		y, // window y
		m_width, // width in px
		m_height, // height in px
		windowFlags
	);
}

Renderer2D* Window_SDL::CreateRenderer()
{
// 		Renderer2DFactory factory(
// 			m_rendererType,
// #ifdef SUPPORT_OPENGL
// 			SDL_GL_GetProcAddress,
// 			[this]()
// 			{
// 				SDL_GL_SwapWindow(this->m_pWindow);
// 			},
// #else
// 			nullptr,
// 			[](){},
// #endif
// #ifdef SUPPORT_VULKAN
// 			[this](unsigned int* extCount, const char** exts)
// 			{
// 				return SDL_Vulkan_GetInstanceExtensions(this->m_pWindow, extCount, exts) == SDL_TRUE;
// 			},
// 			[this](VkInstance instance, VkSurfaceKHR* surface)
// 			{
// 				// return SDL_Vulkan_CreateSurface(this->m_pWindow, (VkInstance)instance, (VkSurfaceKHR*)surface) == SDL_TRUE;
// 				return SDL_Vulkan_CreateSurface(this->m_pWindow, instance, surface) == SDL_TRUE;
// 			}
// #else
// 			[](unsigned int*, const char**){ return false; },
// 			[](VkInstance, VkSurfaceKHR*){ return false; }
// #endif
// 		);
	switch (m_rendererType)
	{
	case RendererBackendType::Unspec:
	case RendererBackendType::SDL2: return CreateSDLRenderer();
	case RendererBackendType::OpenGL: return CreateOpenGLRenderer();
	case RendererBackendType::Vulkan: return CreateVulkanRenderer();
	case RendererBackendType::Metal: return CreateMetalRenderer();
	case RendererBackendType::Unknown: return nullptr;
	}
}

Renderer2D* Window_SDL::CreateSDLRenderer()
{
	if (m_pWindow == nullptr)
		return nullptr;
	if (m_pRenderer != nullptr)
		return m_pRenderer;
	m_pSDLRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_pRenderer = new Renderer2D_SDL(m_pSDLRenderer);
	m_pRenderer->SetViewPortDim({ m_width, m_height });
	return m_pRenderer;
}

Renderer2D* Window_SDL::CreateOpenGLRenderer()
{
#ifdef SUPPORT_OPENGL
	SDL_GLContext context = SDL_GL_CreateContext(m_pWindow);
	if (context == NULL)
		LOG_ERROR("Failed to create OpenGL Context");
	Renderer2D_OpenGL::LoadOpenGLLibrary(SDL_GL_GetProcAddress);
	Renderer2D_OpenGL* openGLRenderer = Renderer2D_OpenGLFactory::CreateOpenGLRenderer();
	openGLRenderer->OnRenderEnd(
		[this]()
		{
			SDL_GL_SwapWindow(this->m_pWindow);
		});
	openGLRenderer->SetViewPortDim({ m_width, m_height });

	m_pRenderer = openGLRenderer;
#else
	throw std::runtime_error("OpenGL renderer is not supported!");
#endif
	return m_pRenderer;
}

Renderer2D* Window_SDL::CreateVulkanRenderer()
{
#ifdef SUPPORT_VULKAN
	auto instanceExtensionGetter = [this](unsigned int* extCount, const char** exts)
	{
		return SDL_Vulkan_GetInstanceExtensions(this->m_pWindow, extCount, exts) == SDL_TRUE;
	};
	auto surfaceCreator = [this](VkInstance instance, VkSurfaceKHR* surface)
	{
		return SDL_Vulkan_CreateSurface(this->m_pWindow, instance, surface) == SDL_TRUE;
	};
	Renderer2D_Vulkan* vulkanRenderer = new Renderer2D_Vulkan();
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

Renderer2D* Window_SDL::CreateMetalRenderer()
{
#ifdef SUPPORT_METAL
	SDL_MetalView view = SDL_Metal_CreateView(m_pWindow);
	m_onDeleteRenderer = [=]() {
		// SDL_Metal_DestroyView(view);
	};
	Renderer2D_Metal* pRenderer = new Renderer2D_Metal(SDL_Metal_GetLayer(view), m_width, m_height);
	m_pRenderer = pRenderer;
	m_pRenderer->SetViewPortDim({ m_width, m_height });
#else
	throw std::runtime_error("Metal renderer is not supported!");
#endif
	return m_pRenderer;
}

Renderer2D* Window_SDL::CreateDirectXRenderer()
{
#ifdef SUPPORT_DIRECTX
#else
	throw std::runtime_error("DirectX renderer is not supported!");
#endif
	return m_pRenderer;
}

void Window_SDL::FreeRenderer()
{
	delete m_pRenderer;
	m_onDeleteRenderer();
}


void Window_SDL::PollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT: 				m_onWindowClose({}); break;
			case SDL_MOUSEMOTION:		m_onMouseMove({ event.motion.x, event.motion.y }); break;
			case SDL_MOUSEBUTTONUP:		m_onMouseButtonUp({ mouseButtonMap[event.button.button] }); break;
			case SDL_MOUSEBUTTONDOWN:	m_onMouseButtonDown({ mouseButtonMap[event.button.button] }); break;
			case SDL_MOUSEWHEEL:		m_onMouseWheel({ event.wheel.x, event.wheel.y }); break;
			case SDL_KEYDOWN:			m_onKeyboardDown({ keyboardMap[event.key.keysym.scancode] }); break;
			case SDL_KEYUP:				m_onKeyboardUp({ keyboardMap[event.key.keysym.scancode] }); break;
			default: break;
		}
	}
	static uint32_t lastticks = SDL_GetTicks();
	uint32_t ticks = SDL_GetTicks();
  	if ( ((ticks*10-lastticks*10)) < 167 )  
    	SDL_Delay( (167-((ticks*10-lastticks*10)))/10 );
  	lastticks = SDL_GetTicks();
}

int Window_SDL::GetWidth() const
{
	return m_width;
}

int Window_SDL::GetHeight() const
{
	return m_height;
}

void Window_SDL::SetTitle(const std::string& title)
{
	SDL_SetWindowTitle(m_pWindow, title.c_str());
}

void Window_SDL::OnWindowClose(WindowCloseCallback&& callback)
{
	m_onWindowClose = callback;
}

void Window_SDL::OnMouseMove(MouseMoveCallback&& callback)
{
	m_onMouseMove = callback;
}

void Window_SDL::OnMouseButtonDown(MouseButtonDownCallback&& callback)
{
	m_onMouseButtonDown = callback;
}

void Window_SDL::OnMouseButtonUp(MouseButtonUpCallback&& callback)
{
	m_onMouseButtonUp = callback;
}

void Window_SDL::OnMouseScrolled(MouseWheelCallback&& callback)
{
	m_onMouseWheel = callback;
}

void Window_SDL::OnKeyboardDown(KeyboardDownCallback&& callback)
{
	m_onKeyboardDown = callback;
}

void Window_SDL::OnKeyboardUp(KeyboardUpCallback&& callback)
{
	m_onKeyboardUp = callback;
}
