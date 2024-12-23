#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

FGraphicsDevice::FGraphicsDevice(const TSharedPtr<FGraphicsContext>& InContext, const TSharedPtr<FGraphicsAdapter>& InAdapter)
	: Context(InContext), GraphicsAdapter(InAdapter)
{
}

FGraphicsDevice::~FGraphicsDevice() = default;

void FGraphicsDevice::Initialize(const TSpan<const char*>& RequiredExtensions)
{
	const TSet<uint32> QueueFamilyIndices = {
		GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Graphics).FamilyIndex,
		GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Present).FamilyIndex,
		GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Compute).FamilyIndex,
		GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Transfer).FamilyIndex,
	};

	uint32 Index = 0;
	constexpr float32 QueuePriorities[] = {1.0f, 1.0f, 1.0f, 1.0f};
	TVector<VkDeviceQueueCreateInfo> QueueCreateInfos(QueueFamilyIndices.size());
	for (const uint32 QueueFamilyIndex : QueueFamilyIndices)
	{
		QueueCreateInfos[Index] = Vulkan::CreateStructure<VkDeviceQueueCreateInfo>();
		QueueCreateInfos[Index].queueFamilyIndex = QueueFamilyIndex;
		QueueCreateInfos[Index].queueCount = GraphicsAdapter->GetQueueCountForIndex(QueueFamilyIndex);
		QueueCreateInfos[Index].pQueuePriorities = QueuePriorities;
		++Index;
	}

	VkDeviceCreateInfo DeviceCreateInfo = Vulkan::CreateStructure<VkDeviceCreateInfo>();
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = nullptr;
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();

	const VkResult Result = vkCreateDevice(GraphicsAdapter->GetHandle(), &DeviceCreateInfo, Context->Allocator, &Context->Device);
	ASSERT(Result == VK_SUCCESS, "Failed to create logical device.");
	volkLoadDevice(Context->Device);
	RW_LOG(LogGraphics, Trace, "Created device");

	GraphicsQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Graphics));
	PresentQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Present));
	ComputeQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Compute));
	TransferQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Transfer));
}

void FGraphicsDevice::Destroy() const
{
	vkDestroyDevice(Context->Device, Context->Allocator);
}

void FGraphicsDevice::WaitForIdle() const
{
	const VkResult Result = vkDeviceWaitIdle(Context->Device);
	ASSERT(Result == VK_SUCCESS, "Failed to wait for device to become idle.");
}

VkQueue FGraphicsDevice::GetQueue(const FGraphicsQueueMetadata& Metadata) const
{
	VkQueue Queue = VK_NULL_HANDLE;
	vkGetDeviceQueue(Context->Device, Metadata.FamilyIndex, Metadata.QueueIndex, &Queue);
	return Queue;
}
