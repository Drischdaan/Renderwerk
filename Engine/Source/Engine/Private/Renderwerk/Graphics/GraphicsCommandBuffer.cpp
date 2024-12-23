#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"

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
