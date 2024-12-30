#include "pch.h"

#include "Renderwerk/Graphics/GraphicsCommandQueue.h"

#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"

FGraphicsCommandQueue::FGraphicsCommandQueue(const EGraphicsQueueType InQueueType, const FGraphicsQueueMetadata InMetadata, const VkQueue InQueue)
	: QueueType(InQueueType), Metadata(InMetadata), Queue(InQueue)
{
}

FGraphicsCommandQueue::~FGraphicsCommandQueue() = default;

void FGraphicsCommandQueue::SubmitCommandBuffers(const TSpan<TSharedPtr<FGraphicsCommandBuffer>>& CommandBuffers, const FGraphicsSubmitSyncDesc& Description) const
{
	TVector<VkCommandBufferSubmitInfo> CommandBufferSubmitInfos;
	CommandBufferSubmitInfos.reserve(CommandBuffers.size());
	for (const TSharedPtr<FGraphicsCommandBuffer>& GraphicsCommandBuffer : CommandBuffers)
	{
		VkCommandBufferSubmitInfo CommandBufferSubmitInfo = Vulkan::CreateStructure<VkCommandBufferSubmitInfo>();
		CommandBufferSubmitInfo.commandBuffer = GraphicsCommandBuffer->CommandBuffer;
		CommandBufferSubmitInfos.push_back(CommandBufferSubmitInfo);
	}

	TVector<VkSemaphoreSubmitInfo> WaitSemaphoreInfos;
	WaitSemaphoreInfos.reserve(Description.WaitSemaphores.size());
	for (const VkSemaphore WaitSemaphore : Description.WaitSemaphores)
	{
		VkSemaphoreSubmitInfo WaitSemaphoreInfo = Vulkan::CreateStructure<VkSemaphoreSubmitInfo>();
		WaitSemaphoreInfo.semaphore = WaitSemaphore;
		WaitSemaphoreInfo.value = 0;
		WaitSemaphoreInfos.push_back(WaitSemaphoreInfo);
	}

	TVector<VkSemaphoreSubmitInfo> SignalSemaphoreInfos;
	SignalSemaphoreInfos.reserve(Description.SignalSemaphores.size());
	for (const VkSemaphore SignalSemaphore : Description.SignalSemaphores)
	{
		VkSemaphoreSubmitInfo SignalSemaphoreInfo = Vulkan::CreateStructure<VkSemaphoreSubmitInfo>();
		SignalSemaphoreInfo.semaphore = SignalSemaphore;
		SignalSemaphoreInfo.value = 0;
		SignalSemaphoreInfos.push_back(SignalSemaphoreInfo);
	}

	VkSubmitInfo2 SubmitInfo = Vulkan::CreateStructure<VkSubmitInfo2>();
	SubmitInfo.commandBufferInfoCount = static_cast<uint32_t>(CommandBufferSubmitInfos.size());
	SubmitInfo.pCommandBufferInfos = CommandBufferSubmitInfos.data();
	SubmitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(WaitSemaphoreInfos.size());
	SubmitInfo.pWaitSemaphoreInfos = WaitSemaphoreInfos.data();
	SubmitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(SignalSemaphoreInfos.size());
	SubmitInfo.pSignalSemaphoreInfos = SignalSemaphoreInfos.data();

	const VkResult Result = vkQueueSubmit2(Queue, 1, &SubmitInfo, Description.SubmitFence);
	ASSERT(Result == VK_SUCCESS, "Failed to submit command buffers to queue");
}
