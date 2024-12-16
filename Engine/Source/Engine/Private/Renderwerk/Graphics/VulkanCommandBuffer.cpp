#include "pch.h"

#include "Renderwerk/Graphics/VulkanCommandBuffer.h"
#include "Renderwerk/Graphics/VulkanGraphicsDevice.h"

FVulkanCommandBuffer::FVulkanCommandBuffer(const TSharedPtr<FVulkanGraphicsDevice>& InDevice, const VkCommandBuffer InCommandBuffer)
	: CommandBuffer(InCommandBuffer)
{
}

FVulkanCommandBuffer::~FVulkanCommandBuffer() = default;

void FVulkanCommandBuffer::Reset() const
{
	const FVulkanResult Result = vkResetCommandBuffer(CommandBuffer, 0);
	VERIFY(Result == VK_SUCCESS, "Failed to reset command buffer");
}

void FVulkanCommandBuffer::Begin() const
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	const FVulkanResult Result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	VERIFY(Result == VK_SUCCESS, "Failed to begin command buffer");
}

void FVulkanCommandBuffer::End() const
{
	const FVulkanResult Result = vkEndCommandBuffer(CommandBuffer);
	VERIFY(Result == VK_SUCCESS, "Failed to end command buffer");
}

void FVulkanCommandBuffer::TransitionImage(const VkImage Image, const VkImageLayout CurrentLayout, const VkImageLayout NewLayout) const
{
	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = (NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	SubImage.baseMipLevel = 0;
	SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
	SubImage.baseArrayLayer = 0;
	SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageMemoryBarrier2 ImageBarrier = {};
	ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	ImageBarrier.pNext = nullptr;
	ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	ImageBarrier.oldLayout = CurrentLayout;
	ImageBarrier.newLayout = NewLayout;
	ImageBarrier.subresourceRange = SubImage;
	ImageBarrier.image = Image;

	VkDependencyInfo DependencyInfo = {};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;
	vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
}

void FVulkanCommandBuffer::ClearImage(const VkImage Image, const VkImageLayout Layout, const VkClearColorValue& ClearValue) const
{
	ClearImage(Image, Layout, VK_IMAGE_ASPECT_COLOR_BIT, ClearValue);
}

void FVulkanCommandBuffer::ClearImage(const VkImage Image, const VkImageLayout Layout, const VkImageAspectFlags& ImageAspectFlags) const
{
	ClearImage(Image, Layout, ImageAspectFlags, {0.0f, 0.0f, 0.0f, 1.0f});
}

void FVulkanCommandBuffer::ClearImage(const VkImage Image, const VkImageLayout Layout, const VkImageAspectFlags& ImageAspectFlags,
                                      const VkClearColorValue& ClearValue) const
{
	VkImageSubresourceRange ClearRange;
	ClearRange.aspectMask = ImageAspectFlags;
	ClearRange.baseMipLevel = 0;
	ClearRange.levelCount = VK_REMAINING_MIP_LEVELS;
	ClearRange.baseArrayLayer = 0;
	ClearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	vkCmdClearColorImage(CommandBuffer, Image, Layout, &ClearValue, 1, &ClearRange);
}

void FVulkanCommandBuffer::BeginDebugLabel(const FString& Name) const
{
	VkDebugUtilsLabelEXT Label = {};
	Label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	Label.pNext = nullptr;
	Label.pLabelName = Name.c_str();
	Label.color[0] = 1.0f;
	Label.color[1] = 1.0f;
	Label.color[2] = 1.0f;
	Label.color[3] = 1.0f;
	vkCmdBeginDebugUtilsLabelEXT(CommandBuffer, &Label);
}

void FVulkanCommandBuffer::EndDebugLabel() const
{
	vkCmdEndDebugUtilsLabelEXT(CommandBuffer);
}
