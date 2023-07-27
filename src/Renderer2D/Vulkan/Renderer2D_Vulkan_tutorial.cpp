#include "VulkanConfig.h"
#if USE_TUTORIAL
#include "Renderer2D_Vulkan.h"
#ifdef SUPPORT_VULKAN
// following tutorial https://vulkan-tutorial.com/en/

// #define PICK_PHYSICAL_DEVICE_V1
#define PICK_PHYSICAL_DEVICE_V2

// #define CLEANUP_V1
#define CLEANUP_V2

#ifdef _WIN32
#endif

#ifdef __APPLE__
#define VK_KHR_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <limits>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <array>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

static const VkClearColorValue backgroundColor{ 0.2f, 0.0f, 0.2f, 0.0f };
static VkClearValue clearColor = { { backgroundColor } };

static const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation",
};

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
	VK_KHR_PORTABILITY_SUBSET_NAME
#endif
};

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif // NDEBUG

/*
* x- x+
* y-
* y+
*/
struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription description{};

		description.binding = 0;
		description.stride = sizeof(Vertex);
		description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		/*
			VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
			VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
		*/

		return description;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> description;
		description[0].binding = 0;
		description[0].location = 0;
		description[0].format = VK_FORMAT_R32G32_SFLOAT;
		description[0].offset = offsetof(Vertex, pos);

		description[1].binding = 0;
		description[1].location = 1;
		description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		description[1].offset = offsetof(Vertex, color);

		return description;
	}
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

static const Vertex vertices[] =
{
	{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
	{ {  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f } },
};

static const uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::string msg_severity = "";
	std::string msg_type = "";
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		msg_severity = "[VERBOSE]";
	}
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		msg_severity = "[INFO]";
	}
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		msg_severity = "[WARNING]";
	}
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		msg_severity = "[ERROR]";
	}

	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
	{
		msg_type = "::GENERAL::";
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
	{
		msg_type = "::VALIDATION::";
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
	{
		msg_type = "::PERFORMANCE::";
	}
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		// message is important enough to show
		std::cerr << "validation layer: "<< msg_severity << msg_type << pCallbackData->pMessage <<std::endl;
	}
	return VK_FALSE;
}

static std::vector<char> readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + fileName);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;

}

Renderer2D_Vulkan::Renderer2D_Vulkan()
{
}

Renderer2D_Vulkan::~Renderer2D_Vulkan()
{
	vkDeviceWaitIdle(m_device);
	cleanup();
}

void Renderer2D_Vulkan::Begin()
{
	vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);

	int result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to recreate swap chain image!");
	}

	// only reset fence if we are submitting work
	if (vkResetFences(m_device, 1, &m_inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("unable to reset fences");
	}

	vkResetCommandBuffer(m_commandBuffer, 0);
}

void Renderer2D_Vulkan::End()
{
	updateUniformBuffer(m_currentImageIndex);
	recordCommandBuffer(m_commandBuffer, m_currentImageIndex);

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = static_cast<uint32_t>(ARRAY_LEN(waitSemaphores));
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffer;
	submitInfo.signalSemaphoreCount = static_cast<uint32_t>(ARRAY_LEN(signalSemaphores));
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkSwapchainKHR swapChains[] = { m_swapChain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = static_cast<uint32_t>(ARRAY_LEN(signalSemaphores));
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = static_cast<uint32_t>(ARRAY_LEN(swapChains));
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &m_currentImageIndex;
	presentInfo.pResults = nullptr;

	int result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized)
	{
		frameBufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	//currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	vkDeviceWaitIdle(m_device);
}

#pragma region COMMON

void Renderer2D_Vulkan::Clear(const Color& color) const
{
	clearColor.color.float32[0] = color.r;
	clearColor.color.float32[1] = color.g;
	clearColor.color.float32[2] = color.b;
	clearColor.color.float32[3] = color.a;
}

void Renderer2D_Vulkan::DrawDisk(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Vulkan::FillCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_Vulkan::FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawLine(const pt2di& start, const pt2di& end, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawText(const pt2di& position, const std::string& text, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const
{

}

void Renderer2D_Vulkan::DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const
{

}

Renderer2D::SpriteID Renderer2D_Vulkan::LoadTexture(const char* path, const std::string& tag)
{
	return 0;
}

std::vector<Renderer2D::SpriteID> Renderer2D_Vulkan::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	return {};
}

bool Renderer2D_Vulkan::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const
{
	return false;
}

bool Renderer2D_Vulkan::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const
{
	return false;
}

vec2di Renderer2D_Vulkan::GetSpriteSize(const std::string& spriteTag) const
{
	return {0, 0};
}

const vec2di& Renderer2D_Vulkan::GetCellDim() const
{
	return m_cellDim;
}

const vec2di& Renderer2D_Vulkan::GetViewPortDim() const
{
	return m_viewPortDim;
}

void Renderer2D_Vulkan::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}

void Renderer2D_Vulkan::SetViewPortDim(const vec2di& dim)
{
	m_viewPortDim = dim;
}
#pragma endregion

void Renderer2D_Vulkan::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffer();
	createSyncObjects();
}

void Renderer2D_Vulkan::cleanup()
{
#if defined CLEANUP_V1
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
	vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
	vkDestroyFence(m_device, m_inFlightFence, nullptr);
	vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	for (auto& framebuffer : m_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}
	vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	for (auto& imageView : m_swapChainImageViews)
	{
		vkDestroyImageView(m_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	vkDestroyDevice(m_device, nullptr);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	delete m_pCallbacks;
#elif defined CLEANUP_V2
	cleanupSwapChain();
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(m_device, m_uniformBuffers[i], nullptr);
		vkFreeMemory(m_device, m_uniformBuffersMemory[i], nullptr);
	}
	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
	vkFreeMemory(m_device, m_indexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
	vkDestroyFence(m_device, m_inFlightFence, nullptr);
	vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	vkDestroyDevice(m_device, nullptr);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
#endif
}

void Renderer2D_Vulkan::createInstance()
{
	// check and enable validation layers
	if (enableValidationLayers && !checkValidationLayerSupport()) 
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}


	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	std::vector<VkExtensionProperties> availableExtensions = getAvailableExtensions();
	std::vector<const char*> requiredExtensions = getRequiredExtensions();

	if (!ExtensionsAreSupported(availableExtensions, requiredExtensions))
	{
		throw std::runtime_error("required extensions not supported");
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
#ifdef __APPLE__
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

std::vector<VkExtensionProperties> Renderer2D_Vulkan::getAvailableExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available Extensions:\n";
	for (const auto& ext : extensions)
	{
		std::cout << '\t' << ext.extensionName << '\n';
	}

	return extensions;
}

std::vector<const char*> Renderer2D_Vulkan::getRequiredExtensions()
{
	uint32_t extensionCount = 0;

	VulkanGetRequiredInstanceExtensions(&extensionCount, nullptr);
	std::vector<const char*> extensions(extensionCount);
	VulkanGetRequiredInstanceExtensions(&extensionCount, extensions.data());

	// std::vector<const char*> extensions(extensionNames, extensionNames + extensionCount);

	if (enableValidationLayers)
	{
		// VK_EXT_DEBUG_UTILS_EXTENSION_NAME macro for "VK_EXT_debug_utils"
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	std::cout << "Required Extensions:\n";
	for (const auto& ext : extensions)
	{
		std::cout << '\t' << ext << '\n';
	}

#ifdef __APPLE__
	std::cout << "MacOS Detected: ext VK_KHR_portability_enumeration required!\n";
	extensions.push_back("VK_KHR_portability_enumeration");
#endif

	return extensions;
}

bool Renderer2D_Vulkan::ExtensionsAreSupported(const std::vector<VkExtensionProperties>& available, const std::vector<const char*>& required)
{
	for (const auto& req : required)
	{
		auto ite = std::find_if(available.begin(), available.end(), [&](const VkExtensionProperties& ext) { return strcmp(req, ext.extensionName) == 0; });
		if (ite == available.end())
		{
			std::cout << "required extension: " << req << " is missing\n";
			return false;
		}
	}
	std::cout << "all required extensions found\n";
	return true;
}

bool Renderer2D_Vulkan::checkValidationLayerSupport() 
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	std::cout << "Available validation layers:\n";
	for (const auto& layer : availableLayers)
	{
		std::cout << '\t' << layer.layerName << ":\n\t\t" << layer.description << '\n';
	}

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerProperties.layerName, layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			std::cout << layerName << std::endl;
			return false;
		}
	}

	return true;
}

void Renderer2D_Vulkan::setupDebugMessenger()
{
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("faied to set up debug messenger!");
	}
}

void Renderer2D_Vulkan::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // optional
}

VkResult Renderer2D_Vulkan::CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Renderer2D_Vulkan::DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT& debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

void Renderer2D_Vulkan::createSurface()
{
#ifdef MANUALLY_CREATE_SURFACE
#ifdef _WIN32
	// win32 surface creation
	HWND hwin = glfwGetWin32Window(m_window);
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = hwin;
	createInfo.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
#endif

#ifdef __APPLE__
	// CGDirectDisplayID glfwGetCocoaMonitor(GLFWmonitor* monitor);
	// id glfwGetCocoaWindow(GLFWwindow* window);
	// if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	// {
	// 	throw std::runtime_error("failed to create window surface!");
	// }
#endif

#else // or let glfw handle it for us
	if (!VulkanCreateSurface(m_instance, &m_surface))
	{
		throw std::runtime_error("failed to create window surface!");
	}
#endif
}

void Renderer2D_Vulkan::pickPhysicalDevice()
{
#if defined PICK_PHYSICAL_DEVICE_V1
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU!");
#elif defined PICK_PHYSICAL_DEVICE_V2
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
	// use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, VkPhysicalDevice> candidates;

	std::cout << "rating GPUs\n";
	for (const auto& device : devices)
	{
		int score = rateDeviceSuitability(device);
		candidates.insert({ score, device });
		//candidates.emplace(score, device);
	}

	if (candidates.rbegin()->first > 0)
		m_physicalDevice = candidates.rbegin()->second;
	else
		throw std::runtime_error("failed to find a suitable GPU!");
#endif
}

bool Renderer2D_Vulkan::isDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	std::cout << deviceProperties.deviceName << std::endl;

	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

	QueueFamilyIndices indices = findQueueFamilies(device);
	bool extensionSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = (!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty());
	}
	return indices.isComplete() && extensionSupported && swapChainAdequate;
}

int Renderer2D_Vulkan::rateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	// Maximum possible size of textures addects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader)
		score += 0;

	std::cout << deviceProperties.deviceName << " -> " << score << std::endl;
	return score;
}

bool Renderer2D_Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

QueueFamilyIndices Renderer2D_Vulkan::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFlags queueFlags = queueFamilies[i].queueFlags;
		if (queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			/*
			VK_QUEUE_COMPUTE_BIT;
			VK_QUEUE_TRANSFER_BIT;
			VK_QUEUE_SPARSE_BINDING_BIT;
			VK_QUEUE_PROTECTED_BIT;
			VK_QUEUE_VIDEO_DECODE_BIT_KHR; // beta
			VK_QUEUE_VIDEO_ENCODE_BIT_KHR; // beta
			VK_QUEUE_FLAG_BITS_MAX_ENUM;
			//*/

			indices.graphicsFamily = i;
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
			}
		}
		if (queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			indices.transferFamiliy = i;
		}

		// we're done
		if (indices.isComplete())
			break;
	}

	return indices;
}

void Renderer2D_Vulkan::createLogicalDevice() 
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.enabledLayerCount = 0;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	// only using index 0 because we're only creating a single queue
	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
	// TODO: try with a transfer queue
}

SwapChainSupportDetails Renderer2D_Vulkan::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Renderer2D_Vulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& format : availableFormats)
	{
#ifdef NON_LINEAR_RGB
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			std::cout << "selected non linear RGB" << std::endl;
			return format;
		}
#else
		if (format.format == VK_FORMAT_B8G8R8A8_UINT &&
			format.colorSpace == VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT)
		{
			std::cout << "selected linear RGB" << std::endl;
			return format;
		}
#endif
	}
	return availableFormats[0];
}

VkPresentModeKHR Renderer2D_Vulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& presentMode : availablePresentModes)
	{
			/*
			VK_PRESENT_MODE_IMMEDIATE_KHR;
			VK_PRESENT_MODE_FIFO_KHR; // good for mobile
			VK_PRESENT_MODE_FIFO_RELAXED_KHR;
			VK_PRESENT_MODE_MAILBOX_KHR;
			*/
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; // only one guaranteed to be available
}

VkExtent2D Renderer2D_Vulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
	if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		int width = m_viewPortDim.w, height = m_viewPortDim.h;

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		return actualExtent;
	}
	return capabilities.currentExtent;
}

void Renderer2D_Vulkan::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //VK_IMAGE_USAGE_TRANSFER_DST_BIT if rendering to a seperate image first

	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; // optional
	createInfo.pQueueFamilyIndices = nullptr; // optional

	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamiliy.value() };
	if (indices.graphicsFamily == indices.presentFamily && indices.graphicsFamily == indices.transferFamiliy)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // optional
		createInfo.pQueueFamilyIndices = nullptr; // optional
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = ARRAY_LEN(queueFamilyIndices);
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = m_swapChain;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void Renderer2D_Vulkan::createImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size());
	for (size_t i = 0; i < m_swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image view: " + std::to_string(i));
		}
	}
}

VkShaderModule Renderer2D_Vulkan::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // TODO: look more into this

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}

void Renderer2D_Vulkan::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // before rendeing
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // after rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void Renderer2D_Vulkan::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void Renderer2D_Vulkan::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/glsl/compiled/vert.spv");
	auto fragShaderCode = readFile("shaders/glsl/compiled/frag.spv");

	std::cout << "vertex shader code size: " << vertShaderCode.size() << std::endl; 
	std::cout << "fragment shader code size: " << fragShaderCode.size() << std::endl;

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = (float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE; // set to VK_TRUE to clamp instead of discarding. Useful for shadow mapping. Requires GPU feature.
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f; // anything other than 1.0f requires wideLines GPU feature.
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	// sometimes used in shadow mapping
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depth{}; // will pass as nullptr for now
	depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	/*
	if (blendEnable) {
		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
	} else {
		finalColor = newColor;
	}
	finalColor = finalColor & colorWriteMask;
	*/

	/*
	if we want alpha blending:
	finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
	finalColor.a = newAlpha.a;

	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	*/

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//std::vector<VkDynamicState> dynamicStates = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(ARRAY_LEN(dynamicStates));
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(ARRAY_LEN(shaderStages));
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

void Renderer2D_Vulkan::createFramebuffers()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = {
			m_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(ARRAY_LEN(attachments));
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer: " + std::to_string(i));
		}
	}
}

void Renderer2D_Vulkan::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);
	
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer2D_Vulkan::createCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_device, &allocInfo, &m_commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Renderer2D_Vulkan::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass;
	renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

	VkBuffer vertexBuffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(m_commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[imageIndex], 0, nullptr);

	vkCmdDrawIndexed(m_commandBuffer, ARRAY_LEN(indices), 1, 0, 0, 0);


	vkCmdEndRenderPass(commandBuffer);
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Renderer2D_Vulkan::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChainExtent.width / (float) m_swapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


void Renderer2D_Vulkan::createSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create synchronization objects!");
	}
}

void Renderer2D_Vulkan::cleanupSwapChain()
{
	for (auto& framebuffer : m_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}
	for (auto& imageView : m_swapChainImageViews)
	{
		vkDestroyImageView(m_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void Renderer2D_Vulkan::recreateSwapChain()
{
	// in case of a resize...
	// int width = 0, height = 0;
	// glfwGetFramebufferSize(m_window, &width, &height);
	// while (width == 0 || height == 0) {
	// 	glfwGetFramebufferSize(m_window, &width, &height);
	// 	glfwWaitEvents();
	// }


	vkDeviceWaitIdle(m_device);
	//cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createFramebuffers();

	// optional: recreate graphics pipeline
	//vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	//createGraphicsPipeline();
}

void Renderer2D_Vulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory &memory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(m_device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_device, buffer, memory, 0);
}

uint32_t Renderer2D_Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type");
}

void Renderer2D_Vulkan::copyBuffer(VkBuffer src, VkBuffer dest, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, src, dest, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void Renderer2D_Vulkan::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices) * sizeof(Vertex);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices, bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vertexBuffer, m_vertexBufferMemory);

	copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void Renderer2D_Vulkan::createIndexBuffer()
{
	VkDeviceSize bufferSize = ARRAY_LEN(indices) * sizeof(indices[0]);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
	stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices, bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	m_indexBuffer, m_indexBufferMemory);

	copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void Renderer2D_Vulkan::createUniformBuffers()
{
	MAX_FRAMES_IN_FLIGHT = m_swapChainFramebuffers.size();

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);
		// we map it immediately. This is known as persistent mapping.
		vkMapMemory(m_device, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
	}
}

void Renderer2D_Vulkan::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("unable to create descriptor pool!");
	}
}

void Renderer2D_Vulkan::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	// need to configure the descriptor sets now...
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject); // can also use VK_WHOLE_SIZE

		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = m_descriptorSets[i];
		writeSet.dstBinding = 0;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeSet.descriptorCount = 1;
		writeSet.pBufferInfo = &bufferInfo;
		writeSet.pImageInfo = nullptr;
		writeSet.pTexelBufferView = nullptr;

		// the VkCopyDescriptorSet is used to copy a descriptor set to another

		vkUpdateDescriptorSets(m_device, 1, &writeSet, 0, nullptr);
	}
}

#endif
#endif