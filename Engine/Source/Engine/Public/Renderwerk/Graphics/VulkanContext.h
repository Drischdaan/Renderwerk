#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FVulkanContext
{
public:
	FVulkanContext() = default;
	~FVulkanContext() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanContext);

public:
	VkAllocationCallbacks* Allocator = nullptr;
	VkInstance Instance = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	TSharedPtr<FVulkanGraphicsDevice> GraphicsDevice = nullptr;
};
