#pragma once

#include "Renderwerk/Graphics/GraphicsDevice.h"

#include "Renderwerk/Graphics/Vulkan/VulkanCommon.h"

class ENGINE_API FVulkanGraphicsDevice : public IGraphicsDevice
{
public:
	FVulkanGraphicsDevice(IGraphicsBackend* InBackend);
	~FVulkanGraphicsDevice() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanGraphicsDevice);

public:
	void Initialize(const TSharedPtr<IGraphicsAdapter>& InGraphicsAdapter) override;
	void Destroy() override;

private:
	VkDevice Device = VK_NULL_HANDLE;
};
