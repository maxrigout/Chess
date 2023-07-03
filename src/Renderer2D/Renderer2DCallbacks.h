#pragma once
#include "Core/PlatformSupport.h"
#include <functional>

#ifdef SUPPORT_VULKAN
#include <vulkan/vulkan.h>
#else
struct VkInstance_T;
typedef VkInstance_T* VkInstance;
struct VkSurfaceKHR;
#endif

using OpenGLProcAddr_t = void *(*)(const char*);
using OpenGLOnRendererEnd_t = std::function<void(void)>;
using VulkanInstanceExtensionGetterFn_t = std::function<bool(unsigned int*, const char**)>;
using VulkanSurfaceCreatorFn_t = std::function<bool(VkInstance, VkSurfaceKHR*)>;