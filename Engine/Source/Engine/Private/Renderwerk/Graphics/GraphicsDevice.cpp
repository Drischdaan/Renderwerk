#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsCommandPool.h"
#include "Renderwerk/Graphics/GraphicsCommandQueue.h"
#include "Renderwerk/Graphics/GraphicsSwapchain.h"

FGraphicsDevice::FGraphicsDevice(const TSharedPtr<FGraphicsContext>& InContext, const TSharedPtr<FGraphicsAdapter>& InAdapter)
	: Context(InContext), GraphicsAdapter(InAdapter)
{
}

FGraphicsDevice::~FGraphicsDevice() = default;

void FGraphicsDevice::Initialize(const TSpan<const char*>& RequiredExtensions, const void* NextChain)
{
	Context->PhysicalDevice = GraphicsAdapter->GetHandle();

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
	DeviceCreateInfo.pNext = NextChain;
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

	GraphicsCommandQueue = CreateQueue(EGraphicsQueueType::Graphics);
	PresentCommandQueue = CreateQueue(EGraphicsQueueType::Present);
	ComputeCommandQueue = CreateQueue(EGraphicsQueueType::Compute);
	TransferCommandQueue = CreateQueue(EGraphicsQueueType::Transfer);
}

void FGraphicsDevice::Destroy()
{
	TransferCommandQueue.reset();
	ComputeCommandQueue.reset();
	PresentCommandQueue.reset();
	GraphicsCommandQueue.reset();
	vkDestroyDevice(Context->Device, Context->Allocator);
}

void FGraphicsDevice::WaitForIdle() const
{
	const VkResult Result = vkDeviceWaitIdle(Context->Device);
	ASSERT(Result == VK_SUCCESS, "Failed to wait for device to become idle.");
}

TSharedPtr<FGraphicsSwapchain> FGraphicsDevice::CreateSwapchain()
{
	return MakeShared<FGraphicsSwapchain>(Context);
}

TSharedPtr<FGraphicsCommandPool> FGraphicsDevice::CreateCommandPool()
{
	return MakeShared<FGraphicsCommandPool>(Context);
}

VkSemaphore FGraphicsDevice::CreateSemaphore() const
{
	const VkSemaphoreCreateInfo SemaphoreCreateInfo = Vulkan::CreateStructure<VkSemaphoreCreateInfo>();

	VkSemaphore Semaphore = VK_NULL_HANDLE;
	const VkResult Result = vkCreateSemaphore(Context->Device, &SemaphoreCreateInfo, Context->Allocator, &Semaphore);
	ASSERT(Result == VK_SUCCESS, "Failed to create semaphore.");
	return Semaphore;
}

void FGraphicsDevice::DestroySemaphore(const VkSemaphore Semaphore) const
{
	vkDestroySemaphore(Context->Device, Semaphore, Context->Allocator);
}

VkFence FGraphicsDevice::CreateFence(const VkFenceCreateFlags Flags) const
{
	VkFenceCreateInfo FenceCreateInfo = Vulkan::CreateStructure<VkFenceCreateInfo>();
	FenceCreateInfo.flags = Flags;

	VkFence Fence = VK_NULL_HANDLE;
	const VkResult Result = vkCreateFence(Context->Device, &FenceCreateInfo, Context->Allocator, &Fence);
	ASSERT(Result == VK_SUCCESS, "Failed to create fence.");
	return Fence;
}

void FGraphicsDevice::DestroyFence(const VkFence Fence) const
{
	vkDestroyFence(Context->Device, Fence, Context->Allocator);
}

TSharedPtr<FGraphicsCommandQueue> FGraphicsDevice::CreateQueue(const EGraphicsQueueType Type) const
{
	FGraphicsQueueMetadata Metadata = GraphicsAdapter->GetQueueMetadata(Type);
	VkQueue Queue = VK_NULL_HANDLE;
	vkGetDeviceQueue(Context->Device, Metadata.FamilyIndex, Metadata.QueueIndex, &Queue);
	return MakeShared<FGraphicsCommandQueue>(EGraphicsQueueType::Transfer, Metadata, Queue);
}
