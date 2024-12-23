#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsCommandPoolDesc
{
	uint32 MaxCommandBuffers = 10;
	uint32 QueueFamilyIndex = 0;
};

class ENGINE_API FGraphicsCommandPool
{
public:
	FGraphicsCommandPool(const TSharedPtr<FGraphicsContext>& InContext);
	~FGraphicsCommandPool();

	DELETE_COPY_AND_MOVE(FGraphicsCommandPool);

public:
	void Initialize(const FGraphicsCommandPoolDesc& InDescription);
	void Destroy() const;

	[[nodiscard]] TSharedPtr<FGraphicsCommandBuffer> AllocateBuffer();
	[[nodiscard]] TVector<TSharedPtr<FGraphicsCommandBuffer>> AllocateBuffers(uint32 Count);

	void FreeBuffer(TSharedPtr<FGraphicsCommandBuffer>& Buffer);
	void FreeBuffers(TVector<TSharedPtr<FGraphicsCommandBuffer>>& Buffers);

private:
	TSharedPtr<FGraphicsContext> Context;

	FGraphicsCommandPoolDesc Description;

	VkCommandPool CommandPool = VK_NULL_HANDLE;
	uint32 CommandBufferUsage = 0;
};
