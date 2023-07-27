#include "VulkanTypeDump.h"

#define DEF_DUMP(vktype) static void dump##vktype(vktype, std::string&);\
std::string dump(vktype type){ std::string out = "[" #vktype "]\n"; dump##vktype(type, out); return out; }\
static void dump##vktype(vktype type, std::string& out)

#define APPEND_FLAG_TYPE(value, str, flag_type) if ((value & flag_type) == flag_type) { str += #flag_type "\n\t"; } do {} while(0)

DEF_DUMP(VkMemoryType)
{
	out += "\tHeapIndex = " + std::to_string(type.heapIndex) + "\n\t";
	APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_PROTECTED_BIT);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD);
    APPEND_FLAG_TYPE(type.propertyFlags, out, VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV);
}

DEF_DUMP(VkMemoryHeap)
{
	out += "\tDevice Size = " + std::to_string(type.size) + "\n\t";
	APPEND_FLAG_TYPE(type.flags, out, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
	APPEND_FLAG_TYPE(type.flags, out, VK_MEMORY_HEAP_MULTI_INSTANCE_BIT);
    APPEND_FLAG_TYPE(type.flags, out, VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR);
}
