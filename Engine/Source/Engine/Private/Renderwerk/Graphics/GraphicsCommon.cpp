#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

#define VOLK_IMPLEMENTATION
#include <volk.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

DEFINE_LOG_CHANNEL(LogGraphics);

VkAllocationCallbacks* GVulkanAllocator = nullptr;

VkAllocationCallbacks* GetVulkanAllocator()
{
	return GVulkanAllocator;
}
