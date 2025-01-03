﻿#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FGraphicsCommandBuffer
{
public:
	FGraphicsCommandBuffer(VkCommandBuffer InCommandBuffer);
	~FGraphicsCommandBuffer();

	DELETE_COPY_AND_MOVE(FGraphicsCommandBuffer);

public:
	void Reset(VkCommandBufferResetFlags Flags = 0) const;
	void Begin() const;
	void End() const;

	void BeginDebugLabel(FStringView Name, const VkClearColorValue& Color = {1.0f, 1.0f, 1.0f, 1.0f}) const;
	void EndDebugLabel() const;

	void TransitionImageLayout(VkImage Image, VkImageLayout OldLayout, VkImageLayout NewLayout, VkImageAspectFlagBits AspectMask = VK_IMAGE_ASPECT_COLOR_BIT) const;

	void ClearImage(VkImage Image, VkImageLayout Layout, const VkClearColorValue& ClearColor = {0.0f, 0.0f, 0.0f, 0.0f},
	                const VkImageAspectFlags& AspectMask = VK_IMAGE_ASPECT_COLOR_BIT) const;

	void ClearRenderTarget(const TSharedPtr<FGraphicsRenderTarget>& RenderTarget) const;

	void BlitImages(VkImage SourceImage, VkImage DestinationImage, VkExtent2D Extent) const;

private:
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	friend class ENGINE_API FGraphicsCommandPool;
	friend class ENGINE_API FGraphicsCommandQueue;
};
