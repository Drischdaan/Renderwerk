#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"
#include "Renderwerk/Graphics/VulkanContext.h"

#include <vulkan/vulkan_core.h>

class ENGINE_API FVulkanGraphicsDevice
{
public:
	FVulkanGraphicsDevice(const FVulkanContext& InContext, const TSharedPtr<FVulkanGraphicsAdapter>& InAdapter);
	~FVulkanGraphicsDevice();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsDevice);

public:
	void CreateDevice();

private:
	FVulkanContext Context;
	TSharedPtr<FVulkanGraphicsAdapter> Adapter;

	VkDevice Device;
};
