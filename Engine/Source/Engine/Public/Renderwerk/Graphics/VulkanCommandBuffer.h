#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FVulkanCommandBuffer
{
public:
	FVulkanCommandBuffer(const TSharedPtr<FVulkanGraphicsDevice>& InDevice, VkCommandBuffer InCommandBuffer);
	~FVulkanCommandBuffer();

	DELETE_COPY_AND_MOVE(FVulkanCommandBuffer);

public:
	void Reset() const;
	void Begin() const;
	void End() const;

	void TransitionImage(VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NewLayout) const;
	void ClearImage(VkImage Image, VkImageLayout Layout, const VkClearColorValue& ClearValue) const;
	void ClearImage(VkImage Image, VkImageLayout Layout, const VkImageAspectFlags& ImageAspectFlags) const;
	void ClearImage(VkImage Image, VkImageLayout Layout, const VkImageAspectFlags& ImageAspectFlags, const VkClearColorValue& ClearValue) const;

	void BeginDebugLabel(const FString& Name) const;
	void EndDebugLabel() const;

private:
	VkCommandBuffer CommandBuffer;

	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;

	friend class ENGINE_API FVulkanGraphicsApi;
};
