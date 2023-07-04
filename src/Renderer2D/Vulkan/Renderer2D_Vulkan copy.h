#if 0
#pragma once
#include "Renderer2D/Renderer2D.h"
#ifdef SUPPORT_VULKAN

#include <vulkan/vulkan.h>
#include <functional>

//#define MANUALLY_CREATE_SURFACE
#define NON_LINEAR_RGB

#if defined __APPLE__
#define VK_KHR_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"
#endif

struct Vertex;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> transferFamiliy;

	bool isComplete()
	{
		return (
			graphicsFamily.has_value() &&
			presentFamily.has_value() &&
			transferFamiliy.has_value());
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct BufferAndMemory
{
	VkBuffer buffer;
	VkDeviceMemory memory;
};

class Renderer2D_Vulkan : public Renderer2D
{
public:
	using VulkanInstanceExtensionGetterFn_t = std::function<bool(unsigned int*, const char**)>;
	using VulkanSurfaceCreatorFn_t = std::function<bool(VkInstance, VkSurfaceKHR*)>;

	Renderer2D_Vulkan();
	virtual ~Renderer2D_Vulkan();

	virtual void Begin() override;
	virtual void End() override;
	virtual void Clear(const Color& color) const override;
	virtual void DrawDisk(const pt2di& center, int radius, const Color& color) const override;
	virtual void DrawCircle(const pt2di& center, int radius, const Color& color) const override;
	virtual void FillCircle(const pt2di& center, int radius, const Color& color) const override;
	virtual void DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override;
	virtual void FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const override;
	virtual void DrawLine(const pt2di& start, const pt2di& end, const Color& color) const override;
	virtual void DrawText(const pt2di& position, const std::string& text, const Color& color) const override;
	virtual void DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const override;
	virtual void DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const override;

	virtual SpriteID LoadTexture(const char* path, const std::string& tag) override;
	virtual std::vector<SpriteID> LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors) override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const override;
	virtual bool DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const override;

	virtual vec2di GetSpriteSize(const std::string& spriteTag) const override;
	virtual const vec2di& GetCellDim() const override;
	virtual const vec2di& GetViewPortDim() const override;
	virtual void SetCellDim(const vec2di& dim) override;
	virtual void SetViewPortDim(const vec2di& dim) override;

	void Init()
	{
		initVulkan();
	}

	void SetInstanceExtensionProvider()
	{
		// TODO
	}
	void SetSurfaceProvider()
	{
		//TODO
	}

	void SetInstanceExtensionGetter(VulkanInstanceExtensionGetterFn_t fn)
	{
		m_getInstanceExtensionFn = std::move(fn);
	}
	void SetSurfaceCreator(VulkanSurfaceCreatorFn_t fn)
	{
		m_createSurfaceFn = std::move(fn);
	}

private:

	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	BufferAndMemory createVertexBuffer(const Vertex* vertices, size_t count);
	BufferAndMemory createIndexBuffer(const uint16_t* indices, size_t count);
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void createSyncObjects();

	void cleanupSwapChain();
	void recreateSwapChain();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator);
	std::vector<VkExtensionProperties> getAvailableExtensions();
	std::vector<const char*> getRequiredExtensions();
	bool ExtensionsAreSupported(const std::vector<VkExtensionProperties>& available, const std::vector<const char*>& required);
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory &memory);
	void copyBuffer(VkBuffer src, VkBuffer dest, VkDeviceSize size);

	void initVulkan();
	void cleanup();

	bool VulkanGetRequiredInstanceExtensions(unsigned int* count, const char** extensions)
	{
		return m_getInstanceExtensionFn(count, extensions);
	}
	bool VulkanCreateSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		return m_createSurfaceFn(instance, surface);
	}


	VulkanInstanceExtensionGetterFn_t m_getInstanceExtensionFn;
	VulkanSurfaceCreatorFn_t m_createSurfaceFn;

	vec2di m_windowDim{};
	vec2di m_cellDim{};
	vec2di m_viewPortDim{};

	uint32_t m_currentImageIndex;

	VkAllocationCallbacks* m_pCallbacks;

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device;

	VkQueue m_graphicsQueue;
	VkSurfaceKHR m_surface;
	VkQueue m_presentQueue;

	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	std::vector<VkImageView> m_swapChainImageViews;
	
	VkRenderPass m_renderPass;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;

	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;

	VkSemaphore m_imageAvailableSemaphore;
	VkSemaphore m_renderFinishedSemaphore;
	VkFence m_inFlightFence;

	bool frameBufferResized = false;

	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;

	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;

	BufferAndMemory quadIndices;
	BufferAndMemory quad1;
	BufferAndMemory quad2;
};

#endif
#endif