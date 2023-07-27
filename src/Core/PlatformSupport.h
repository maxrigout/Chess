#pragma once
#if defined _WIN32
#define SUPPORT_OPENGL
#define SUPPORT_VULKAN
#define SUPPORT_DIRECTX
#define SUPPORT_SDL
#define SUPPORT_GLFW
#define SUPPORT_WIN32
#elif defined __APPLE__
#define SUPPORT_OPENGL
#define SUPPORT_VULKAN
#define SUPPORT_METAL
#define SUPPORT_SDL
#define SUPPORT_GLFW
#elif defined __linux__
#define SUPPORT_OPENGL
#define SUPPORT_VULKAN
#define SUPPORT_SDL
#define SUPPORT_GLFW
#endif