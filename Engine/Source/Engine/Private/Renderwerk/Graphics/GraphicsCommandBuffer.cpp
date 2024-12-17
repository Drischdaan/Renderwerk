#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"

FGraphicsCommandBuffer::FGraphicsCommandBuffer(const VkCommandBuffer& InCommandBuffer)
	: CommandBuffer(InCommandBuffer)
{
}

FGraphicsCommandBuffer::~FGraphicsCommandBuffer() = default;

void FGraphicsCommandBuffer::Reset() const
{
	const FVulkanResult Result = vkResetCommandBuffer(CommandBuffer, 0);
	VERIFY(Result == VK_SUCCESS, "Failed to reset command buffer");
}

void FGraphicsCommandBuffer::Begin() const
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;

	const FVulkanResult Result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	VERIFY(Result == VK_SUCCESS, "Failed to begin command buffer");
}

void FGraphicsCommandBuffer::End() const
{
	const FVulkanResult Result = vkEndCommandBuffer(CommandBuffer);
	VERIFY(Result == VK_SUCCESS, "Failed to end command buffer");
}

void FGraphicsCommandBuffer::TransitionImageLayout(const VkImage& Image, const VkImageLayout OldLayout, const VkImageLayout NewLayout) const
{
	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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
	ImageBarrier.oldLayout = OldLayout;
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

void FGraphicsCommandBuffer::ClearImage(const VkImage& Image, const VkImageLayout Layout, const VkClearColorValue& ClearColor) const
{
	VkImageSubresourceRange SubresourceRange;
	SubresourceRange.aspectMask = Layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	SubresourceRange.baseMipLevel = 0;
	SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	SubresourceRange.baseArrayLayer = 0;
	SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	vkCmdClearColorImage(CommandBuffer, Image, Layout, &ClearColor, 1, &SubresourceRange);
}

void FGraphicsCommandBuffer::BeginDebugLabel(const FString& Name, const VkClearColorValue& Color) const
{
	VkDebugUtilsLabelEXT Label = {};
	Label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	Label.pNext = nullptr;
	Label.pLabelName = Name.c_str();
	Label.color[0] = Color.float32[0];
	Label.color[1] = Color.float32[1];
	Label.color[2] = Color.float32[2];
	Label.color[3] = Color.float32[3];
	vkCmdBeginDebugUtilsLabelEXT(CommandBuffer, &Label);
}

void FGraphicsCommandBuffer::EndDebugLabel() const
{
	vkCmdEndDebugUtilsLabelEXT(CommandBuffer);
}
