#include <vulkan/vulkan.h>
#include <string>

#define DECL_DUMP(type) std::string dump(type)

DECL_DUMP(VkMemoryType);
DECL_DUMP(VkMemoryHeap);
