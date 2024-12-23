#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommandPool.h"

#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"

FGraphicsCommandPool::FGraphicsCommandPool(const TSharedPtr<FGraphicsContext>& InContext)
	: Context(InContext)
{
}

FGraphicsCommandPool::~FGraphicsCommandPool() = default;

void FGraphicsCommandPool::Initialize(const FGraphicsCommandPoolDesc& InDescription)
{
	Description = InDescription;

	VkCommandPoolCreateInfo CommandPoolCreateInfo = Vulkan::CreateStructure<VkCommandPoolCreateInfo>();
	CommandPoolCreateInfo.queueFamilyIndex = Description.QueueFamilyIndex;
	CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	const VkResult Result = vkCreateCommandPool(Context->Device, &CommandPoolCreateInfo, Context->Allocator, &CommandPool);
	ASSERT(Result == VK_SUCCESS, "Failed to create command pool");
}

void FGraphicsCommandPool::Destroy() const
{
	vkDestroyCommandPool(Context->Device, CommandPool, Context->Allocator);
}

TSharedPtr<FGraphicsCommandBuffer> FGraphicsCommandPool::AllocateBuffer()
{
	ASSERT(CommandBufferUsage < Description.MaxCommandBuffers, "Command buffer limit reached");

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = Vulkan::CreateStructure<VkCommandBufferAllocateInfo>();
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	const VkResult Result = vkAllocateCommandBuffers(Context->Device, &CommandBufferAllocateInfo, &CommandBuffer);
	ASSERT(Result == VK_SUCCESS, "Failed to allocate command buffer");
	++CommandBufferUsage;
	RW_LOG(LogGraphics, Debug, "Allocated command buffer. Usage: {}/{}", CommandBufferUsage, Description.MaxCommandBuffers);
	return MakeShared<FGraphicsCommandBuffer>(CommandBuffer);
}

TVector<TSharedPtr<FGraphicsCommandBuffer>> FGraphicsCommandPool::AllocateBuffers(const uint32 Count)
{
	ASSERT(CommandBufferUsage + Count <= Description.MaxCommandBuffers, "Command buffer limit reached");

	TVector<VkCommandBuffer> CommandBuffers;
	CommandBuffers.resize(Count);

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = Vulkan::CreateStructure<VkCommandBufferAllocateInfo>();
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = Count;

	const VkResult Result = vkAllocateCommandBuffers(Context->Device, &CommandBufferAllocateInfo, CommandBuffers.data());
	ASSERT(Result == VK_SUCCESS, "Failed to allocate command buffers");
	CommandBufferUsage += Count;

	TVector<TSharedPtr<FGraphicsCommandBuffer>> GraphicsCommandBuffers;
	GraphicsCommandBuffers.reserve(Count);
	std::ranges::transform(CommandBuffers, std::back_inserter(GraphicsCommandBuffers), [](VkCommandBuffer CommandBuffer)
	{
		return MakeShared<FGraphicsCommandBuffer>(CommandBuffer);
	});
	RW_LOG(LogGraphics, Debug, "Allocated {} command buffers. Usage: {}/{}", Count, CommandBufferUsage, Description.MaxCommandBuffers);
	return GraphicsCommandBuffers;
}

void FGraphicsCommandPool::FreeBuffer(TSharedPtr<FGraphicsCommandBuffer>& Buffer)
{
	vkFreeCommandBuffers(Context->Device, CommandPool, 1, &Buffer->CommandBuffer);
	Buffer.reset();
	--CommandBufferUsage;
}

void FGraphicsCommandPool::FreeBuffers(TVector<TSharedPtr<FGraphicsCommandBuffer>>& Buffers)
{
	TVector<VkCommandBuffer> CommandBuffers;
	CommandBuffers.reserve(Buffers.size());
	std::ranges::transform(Buffers, std::back_inserter(CommandBuffers), [](const TSharedPtr<FGraphicsCommandBuffer>& Buffer)
	{
		return Buffer->CommandBuffer;
	});
	Buffers.clear();
	vkFreeCommandBuffers(Context->Device, CommandPool, static_cast<uint32>(CommandBuffers.size()), CommandBuffers.data());
	CommandBufferUsage -= static_cast<uint32>(CommandBuffers.size());
}
