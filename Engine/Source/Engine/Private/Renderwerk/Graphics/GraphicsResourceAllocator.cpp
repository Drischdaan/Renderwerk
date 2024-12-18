#include "pch.h"

#include "Renderwerk/Graphics/GraphicsResourceAllocator.h"

FGraphicsResourceAllocator::FGraphicsResourceAllocator(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsDevice>& InGraphicsDevice)
	: GraphicsContext(InGraphicsContext), GraphicsDevice(InGraphicsDevice)
{
}

FGraphicsResourceAllocator::~FGraphicsResourceAllocator() = default;

void FGraphicsResourceAllocator::Initialize()
{
	VmaVulkanFunctions VulkanFunctions = {};
	VulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	VulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.instance = GraphicsContext->GetInstance();
	AllocatorCreateInfo.physicalDevice = GraphicsDevice->GetAdapter()->GetHandle();
	AllocatorCreateInfo.device = GraphicsDevice->GetHandle();
	AllocatorCreateInfo.pAllocationCallbacks = GraphicsContext->GetAllocator();
	AllocatorCreateInfo.vulkanApiVersion = GraphicsDevice->GetAdapter()->GetApiVersion();
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	AllocatorCreateInfo.pVulkanFunctions = &VulkanFunctions;

	const FVulkanResult Result = vmaCreateAllocator(&AllocatorCreateInfo, &Allocator);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan memory allocator");
}

void FGraphicsResourceAllocator::Destroy() const
{
	vmaDestroyAllocator(Allocator);
}

FGraphicsBuffer FGraphicsResourceAllocator::AllocateBuffer(const size64 Size, const VkBufferUsageFlags Usage, const VmaMemoryUsage MemoryUsage) const
{
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = Usage;

	VmaAllocationCreateInfo AllocationCreateInfo = {};
	AllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocationCreateInfo.usage = MemoryUsage;

	FGraphicsBuffer Buffer = {};
	const FVulkanResult Result = vmaCreateBuffer(Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer.Buffer, &Buffer.Allocation, &Buffer.AllocationInfo);
	VERIFY(Result == VK_SUCCESS, "Failed to create buffer");
	return Buffer;
}

void FGraphicsResourceAllocator::FreeBuffer(const FGraphicsBuffer& Buffer) const
{
	vmaDestroyBuffer(Allocator, Buffer.Buffer, Buffer.Allocation);
}

FGraphicsBuffer FGraphicsResourceAllocator::CreateUploadBuffer(const TSharedPtr<FGraphicsDevice>& Device, const FGraphicsBuffer& Buffer, const void* Data,
                                                               const size64 Size) const
{
	FGraphicsBuffer GpuBuffer = AllocateBuffer(Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	                                           VMA_MEMORY_USAGE_GPU_ONLY);
	VkBufferDeviceAddressInfo DeviceAddressInfo{};
	DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	DeviceAddressInfo.buffer = GpuBuffer.Buffer;
	GpuBuffer.DeviceAddress = vkGetBufferDeviceAddress(Device->GetHandle(), &DeviceAddressInfo);

	void* MappedData = nullptr;
	const FVulkanResult Result = vmaMapMemory(Allocator, Buffer.Allocation, &MappedData);
	VERIFY(Result == VK_SUCCESS, "Failed to map memory");
	FMemory::Copy(MappedData, Data, Size);
	vmaUnmapMemory(Allocator, Buffer.Allocation);
	return GpuBuffer;
}
