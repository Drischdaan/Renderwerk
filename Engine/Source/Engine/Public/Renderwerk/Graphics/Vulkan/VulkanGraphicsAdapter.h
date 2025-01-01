#pragma once

#include "Renderwerk/Graphics/GraphicsAdapter.h"

#include "Renderwerk/Graphics/Vulkan/VulkanCommon.h"

class FVulkanGraphicsBackend;

class ENGINE_API FVulkanGraphicsAdapter : public IGraphicsAdapter
{
public:
	FVulkanGraphicsAdapter(IGraphicsBackend* InBackend, VkPhysicalDevice InPhysicalDevice);
	~FVulkanGraphicsAdapter() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanGraphicsAdapter);

public:
	void Initialize() override;
	void Destroy() override;

public:
	[[nodiscard]] uint32 GetApiVersion() const { return ApiVersion; }

private:
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	uint32 ApiVersion = 0;
};
