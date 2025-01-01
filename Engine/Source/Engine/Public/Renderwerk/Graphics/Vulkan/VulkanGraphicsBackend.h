#pragma once

#include "Renderwerk/Graphics/GraphicsBackend.h"

#include "Renderwerk/Graphics/Vulkan/VulkanCommon.h"

class ENGINE_API FVulkanGraphicsBackend : public IGraphicsBackend
{
public:
	FVulkanGraphicsBackend();
	~FVulkanGraphicsBackend() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanGraphicsBackend);

public:
	void Initialize(const FGraphicsBackendDesc& InDescription) override;
	void Destroy() override;

	[[nodiscard]] TVector<TSharedPtr<IGraphicsAdapter>> GetAvailableAdapters() override;

public:
	[[nodiscard]] VkAllocationCallbacks* GetVulkanAllocator() const { return VulkanAllocator; }
	[[nodiscard]] VkInstance GetVulkanInstance() const { return VulkanInstance; }

private:
	VkAllocationCallbacks* VulkanAllocator = nullptr;
	VkInstance VulkanInstance = VK_NULL_HANDLE;
};
