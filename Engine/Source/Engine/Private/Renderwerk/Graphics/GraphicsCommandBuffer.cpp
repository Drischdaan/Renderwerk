#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"

#include "Renderwerk/Graphics/GraphicsRenderTarget.h"

FGraphicsCommandBuffer::FGraphicsCommandBuffer(const VkCommandBuffer InCommandBuffer)
	: CommandBuffer(InCommandBuffer)
{
}

FGraphicsCommandBuffer::~FGraphicsCommandBuffer() = default;

void FGraphicsCommandBuffer::Reset(const VkCommandBufferResetFlags Flags) const
{
	const VkResult Result = vkResetCommandBuffer(CommandBuffer, Flags);
	ASSERT(Result == VK_SUCCESS, "Failed to reset command buffer!");
}

void FGraphicsCommandBuffer::Begin() const
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo = Vulkan::CreateStructure<VkCommandBufferBeginInfo>();
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;

	const VkResult Result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	ASSERT(Result == VK_SUCCESS, "Failed to begin command buffer!");
}

void FGraphicsCommandBuffer::End() const
{
	const VkResult Result = vkEndCommandBuffer(CommandBuffer);
	ASSERT(Result == VK_SUCCESS, "Failed to end command buffer!");
}

void FGraphicsCommandBuffer::BeginDebugLabel(const FStringView Name, const VkClearColorValue& Color) const
{
	VkDebugUtilsLabelEXT DebugLabel = Vulkan::CreateStructure<VkDebugUtilsLabelEXT>();
	DebugLabel.pLabelName = Name.data();
	DebugLabel.color[0] = Color.float32[0];
	DebugLabel.color[1] = Color.float32[1];
	DebugLabel.color[2] = Color.float32[2];
	DebugLabel.color[3] = Color.float32[3];
	vkCmdBeginDebugUtilsLabelEXT(CommandBuffer, &DebugLabel);
}

void FGraphicsCommandBuffer::EndDebugLabel() const
{
	vkCmdEndDebugUtilsLabelEXT(CommandBuffer);
}

void FGraphicsCommandBuffer::TransitionImageLayout(const VkImage Image, const VkImageLayout OldLayout, const VkImageLayout NewLayout,
                                                   const VkImageAspectFlagBits AspectMask) const
{
	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = AspectMask;
	SubImage.baseMipLevel = 0;
	SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
	SubImage.baseArrayLayer = 0;
	SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageMemoryBarrier2 ImageMemoryBarrier = Vulkan::CreateStructure<VkImageMemoryBarrier2>();
	ImageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageMemoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	ImageMemoryBarrier.oldLayout = OldLayout;
	ImageMemoryBarrier.newLayout = NewLayout;
	ImageMemoryBarrier.subresourceRange = SubImage;
	ImageMemoryBarrier.image = Image;

	VkDependencyInfo DependencyInfo = Vulkan::CreateStructure<VkDependencyInfo>();
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;
	vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
}

void FGraphicsCommandBuffer::ClearImage(const VkImage Image, const VkImageLayout Layout, const VkClearColorValue& ClearColor, const VkImageAspectFlags& AspectMask) const
{
	VkImageSubresourceRange SubresourceRange;
	SubresourceRange.aspectMask = AspectMask;
	SubresourceRange.baseMipLevel = 0;
	SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	SubresourceRange.baseArrayLayer = 0;
	SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	vkCmdClearColorImage(CommandBuffer, Image, Layout, &ClearColor, 1, &SubresourceRange);
}

void FGraphicsCommandBuffer::ClearRenderTarget(const TSharedPtr<FGraphicsRenderTarget>& RenderTarget) const
{
	VkImageSubresourceRange SubresourceRange;
	SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	SubresourceRange.baseMipLevel = 0;
	SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	SubresourceRange.baseArrayLayer = 0;
	SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	const VkClearColorValue ClearColor = RenderTarget->GetClearColor();
	vkCmdClearColorImage(CommandBuffer, RenderTarget->GetHandle(), VK_IMAGE_LAYOUT_GENERAL, &ClearColor, 1, &SubresourceRange);
}

void FGraphicsCommandBuffer::BlitImages(const VkImage SourceImage, const VkImage DestinationImage, const VkExtent2D Extent) const
{
	VkImageBlit ImageBlit = {};
	ImageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageBlit.srcSubresource.layerCount = 1;
	ImageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageBlit.dstSubresource.layerCount = 1;
	ImageBlit.srcOffsets[0].x = 0;
	ImageBlit.srcOffsets[0].y = 0;
	ImageBlit.srcOffsets[0].z = 0;
	ImageBlit.srcOffsets[1].x = static_cast<int32_t>(Extent.width);
	ImageBlit.srcOffsets[1].y = static_cast<int32_t>(Extent.height);
	ImageBlit.srcOffsets[1].z = 1;
	ImageBlit.dstOffsets[0].x = 0;
	ImageBlit.dstOffsets[0].y = 0;
	ImageBlit.dstOffsets[0].z = 0;
	ImageBlit.dstOffsets[1].x = static_cast<int32_t>(Extent.width);
	ImageBlit.dstOffsets[1].y = static_cast<int32_t>(Extent.height);
	ImageBlit.dstOffsets[1].z = 1;
	vkCmdBlitImage(CommandBuffer, SourceImage, VK_IMAGE_LAYOUT_GENERAL, DestinationImage, VK_IMAGE_LAYOUT_GENERAL, 1, &ImageBlit,
	               VK_FILTER_LINEAR);
}
