#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsBuffer
{
public:
	FGraphicsBuffer() = default;
	FGraphicsBuffer(VkBuffer Buffer, VmaAllocation Allocation, const VmaAllocationInfo& AllocationInfo);
	~FGraphicsBuffer() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsBuffer);

public:
	[[nodiscard]] VkBuffer GetHandle() const { return Buffer; }
	[[nodiscard]] VkDeviceAddress GetDeviceAddress() const { return DeviceAddress; }

private:
	VkBuffer Buffer = VK_NULL_HANDLE;
	VmaAllocation Allocation = VK_NULL_HANDLE;
	VmaAllocationInfo AllocationInfo = {};
	VkDeviceAddress DeviceAddress = 0;

	friend class ENGINE_API FGraphicsResourceAllocator;
};
