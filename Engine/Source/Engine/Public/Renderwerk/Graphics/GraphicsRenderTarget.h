#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsRenderTargetDesc
{
	uint32 Width = 100;
	uint32 Height = 100;
	VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM;
	EGraphicsMemoryUsage MemoryUsage = EGraphicsMemoryUsage::GPU;
	VkImageAspectFlagBits AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	VkClearColorValue ClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
};

class ENGINE_API FGraphicsRenderTarget
{
public:
	FGraphicsRenderTarget(const TSharedPtr<FGraphicsContext>& InContext);
	~FGraphicsRenderTarget();

	DELETE_COPY_AND_MOVE(FGraphicsRenderTarget);

public:
	void Initialize(const FGraphicsRenderTargetDesc& InDescription);
	void Destroy() const;

public:
	[[nodiscard]] VkImage GetHandle() const { return Image; }
	[[nodiscard]] VkImageView GetHandleView() const { return ImageView; }

	[[nodiscard]] VkClearColorValue GetClearColor() const { return Description.ClearColor; }

private:
	TSharedPtr<FGraphicsContext> Context;

	FGraphicsRenderTargetDesc Description;

	VkImage Image = VK_NULL_HANDLE;
	VkImageView ImageView = VK_NULL_HANDLE;
	VmaAllocation Allocation = VK_NULL_HANDLE;
	VmaAllocationInfo AllocationInfo = {};
};
