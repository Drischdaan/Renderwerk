#include "pch.h"

#include "Renderwerk/Graphics/GraphicsBuffer.h"

namespace
{
	VkBufferUsageFlagBits ConvertBufferUsage(const EGraphicsBufferUsage Usage)
	{
		switch (Usage)
		{
		case EGraphicsBufferUsage::Vertex:
			return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case EGraphicsBufferUsage::Index:
			return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		case EGraphicsBufferUsage::Storage:
			return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		default:
			return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
		}
	}

	VmaMemoryUsage ConvertMemoryUsage(const EGraphicsMemoryUsage Usage)
	{
		switch (Usage)
		{
		case EGraphicsMemoryUsage::Auto:
			return VMA_MEMORY_USAGE_AUTO;
		case EGraphicsMemoryUsage::CPU:
			return VMA_MEMORY_USAGE_CPU_ONLY;
		case EGraphicsMemoryUsage::GPU:
			return VMA_MEMORY_USAGE_GPU_ONLY;
		default:
			return VMA_MEMORY_USAGE_UNKNOWN;
		}
	}
}

FGraphicsBuffer::FGraphicsBuffer(const TSharedPtr<FGraphicsContext>& InContext)
	: Context(InContext)
{
}

FGraphicsBuffer::~FGraphicsBuffer() = default;

void FGraphicsBuffer::Initialize(const FGraphicsBufferDesc& InDescription)
{
	VkBufferCreateInfo BufferCreateInfo = Vulkan::CreateStructure<VkBufferCreateInfo>();
	BufferCreateInfo.size = InDescription.Size;
	BufferCreateInfo.usage = ConvertBufferUsage(InDescription.Usage);
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo AllocationCreateInfo = Vulkan::CreateStructure<VmaAllocationCreateInfo>();
	AllocationCreateInfo.usage = ConvertMemoryUsage(InDescription.MemoryUsage);

	const VkResult Result = vmaCreateBuffer(Context->ResourceAllocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer, &Allocation, &AllocationInfo);
	ASSERT(Result == VK_SUCCESS, "Failed to create buffer");
}

void FGraphicsBuffer::Destroy() const
{
	vmaDestroyBuffer(Context->ResourceAllocator, Buffer, Allocation);
}

void FGraphicsBuffer::CopyData(const void* Data, const size64 Size) const
{
	void* MappedData = nullptr;
	const VkResult Result = vmaMapMemory(Context->ResourceAllocator, Allocation, &MappedData);
	ASSERT(Result == VK_SUCCESS, "Failed to map buffer memory");
	FMemory::Copy(MappedData, Data, Size);
	vmaUnmapMemory(Context->ResourceAllocator, Allocation);
}
