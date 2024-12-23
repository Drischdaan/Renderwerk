#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"
#include "Renderwerk/Graphics/VulkanStructureInfo.h"

#include <volk.h>

DECLARE_LOG_CHANNEL(LogGraphics);

struct ENGINE_API FGraphicsContext
{
	VkAllocationCallbacks* Allocator = nullptr;
	VkInstance Instance = VK_NULL_HANDLE;
	VkDevice Device = VK_NULL_HANDLE;
};

enum class ENGINE_API EGraphicsQueueType : uint8
{
	None = 0,
	Graphics,
	Present,
	Compute,
	Transfer,
};

struct ENGINE_API FGraphicsQueueMetadata
{
	uint32 FamilyIndex = 0;
	uint32 QueueIndex = 0;
};

struct ENGINE_API FGraphicsSurfaceCapabilities
{
	VkSurfaceCapabilitiesKHR Capabilities = {};
	TVector<VkSurfaceFormatKHR> Formats = {};
	TVector<VkPresentModeKHR> PresentModes = {};
};
