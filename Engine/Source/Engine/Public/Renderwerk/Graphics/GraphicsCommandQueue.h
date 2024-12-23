#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsSubmitSyncDesc
{
	TVector<VkSemaphore> WaitSemaphores;
	TVector<VkSemaphore> SignalSemaphores;
	VkFence SubmitFence;
};

class ENGINE_API FGraphicsCommandQueue
{
public:
	FGraphicsCommandQueue(EGraphicsQueueType InQueueType, FGraphicsQueueMetadata InMetadata, VkQueue InQueue);
	~FGraphicsCommandQueue();

	DELETE_COPY_AND_MOVE(FGraphicsCommandQueue);

public:
	void SubmitCommandBuffers(const TSpan<TSharedPtr<FGraphicsCommandBuffer>>& CommandBuffers, const FGraphicsSubmitSyncDesc& Description = {}) const;

public:
	[[nodiscard]] VkQueue GetHandle() const { return Queue; }

private:
	EGraphicsQueueType QueueType;
	FGraphicsQueueMetadata Metadata;
	VkQueue Queue;
};
