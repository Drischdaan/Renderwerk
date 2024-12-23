#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

enum class ENGINE_API EGraphicsBufferUsage : uint8
{
	None = 0,
	Vertex,
	Index,
	Storage,
};

struct ENGINE_API FGraphicsBufferDesc
{
	size64 Size;
	EGraphicsBufferUsage Usage;
	EGraphicsMemoryUsage MemoryUsage = EGraphicsMemoryUsage::Auto;
};

class ENGINE_API FGraphicsBuffer
{
public:
	FGraphicsBuffer(const TSharedPtr<FGraphicsContext>& InContext);
	~FGraphicsBuffer();

	DELETE_COPY_AND_MOVE(FGraphicsBuffer);

public:
	void Initialize(const FGraphicsBufferDesc& InDescription);
	void Destroy() const;

	void CopyData(const void* Data, size64 Size) const;

public:
	[[nodiscard]] VkBuffer GetHandle() const { return Buffer; }

private:
	TSharedPtr<FGraphicsContext> Context;

	FGraphicsBufferDesc Description = {};

	VkBuffer Buffer = VK_NULL_HANDLE;
	VmaAllocation Allocation = VK_NULL_HANDLE;
	VmaAllocationInfo AllocationInfo = {};
};
