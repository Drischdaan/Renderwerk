#pragma once

#include "Renderwerk/Graphics/GraphicsWindowContext.h"

#include "Renderwerk/Graphics/Vulkan/VulkanCommon.h"

class ENGINE_API FVulkanGraphicsWindowContext : public IGraphicsWindowContext
{
public:
	FVulkanGraphicsWindowContext(IGraphicsBackend* InBackend);
	~FVulkanGraphicsWindowContext() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanGraphicsWindowContext);

public:
	void Initialize(const TSharedPtr<FWindow>& InWindow) override;
	void Destroy() override;

public:
	[[nodiscard]] VkSurfaceKHR GetSurface() const { return Surface; }

private:
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
};
