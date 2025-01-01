#pragma once

#include "Renderwerk/Graphics/GraphicsAdapter.h"

#include "Renderwerk/Graphics/Vulkan/VulkanCommon.h"

class FVulkanGraphicsBackend;

struct ENGINE_API FVulkanQueueMetadata
{
	uint32 FamilyIndex = 0;
	uint32 QueueIndex = 0;
};

class ENGINE_API FVulkanGraphicsAdapter : public IGraphicsAdapter
{
public:
	FVulkanGraphicsAdapter(IGraphicsBackend* InBackend, VkPhysicalDevice InPhysicalDevice);
	~FVulkanGraphicsAdapter() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanGraphicsAdapter);

public:
	void Initialize(const TSharedPtr<IGraphicsWindowContext>& InGraphicsWindowContext) override;
	void Destroy() override;

public:
	[[nodiscard]] bool8 SupportsLayer(const FString& LayerName) const;
	[[nodiscard]] bool8 SupportsExtension(const FString& ExtensionName) const;

	[[nodiscard]] FVulkanQueueMetadata GetQueueMetadata(EGraphicsQueueType Type) const;
	[[nodiscard]] uint32 GetQueueCountForIndex(uint32 QueueFamilyIndex) const;

public:
	[[nodiscard]] VkPhysicalDevice GetHandle() const { return PhysicalDevice; }

	[[nodiscard]] uint32 GetApiVersion() const { return ApiVersion; }

private:
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	uint32 ApiVersion = 0;

	TVector<FString> Layers;
	TVector<FString> Extensions;

	TMap<EGraphicsQueueType, FVulkanQueueMetadata> QueueMetadataMap;
};
