#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsBuffer.h"
#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

class ENGINE_API FGraphicsResourceAllocator
{
public:
	FGraphicsResourceAllocator(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsDevice>& InGraphicsDevice);
	~FGraphicsResourceAllocator();

	DELETE_COPY_AND_MOVE(FGraphicsResourceAllocator);

public:
	void Initialize();
	void Destroy() const;

	[[nodiscard]] FGraphicsBuffer AllocateBuffer(size64 Size, VkBufferUsageFlags Usage, VmaMemoryUsage MemoryUsage) const;
	void FreeBuffer(const FGraphicsBuffer& Buffer) const;

	FGraphicsBuffer CreateUploadBuffer(const TSharedPtr<FGraphicsDevice>& Device, const FGraphicsBuffer& Buffer, const void* Data, size64 Size,
	                                   VkBufferUsageFlags Usage) const;

public:
	[[nodiscard]] VmaAllocator GetHandle() const { return Allocator; }

private:
	TSharedPtr<FGraphicsContext> GraphicsContext;
	TSharedPtr<FGraphicsDevice> GraphicsDevice;

	VmaAllocator Allocator = VK_NULL_HANDLE;
};
