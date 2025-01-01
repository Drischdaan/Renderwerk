#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsDevice.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsAdapter.h"
#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"

FVulkanGraphicsDevice::FVulkanGraphicsDevice(IGraphicsBackend* InBackend)
	: IGraphicsDevice(InBackend)
{
}

FVulkanGraphicsDevice::~FVulkanGraphicsDevice() = default;

void FVulkanGraphicsDevice::Initialize(const TSharedPtr<IGraphicsAdapter>& InGraphicsAdapter)
{
	GraphicsAdapter = InGraphicsAdapter;

	const FVulkanGraphicsBackend& VulkanBackend = Backend->As<FVulkanGraphicsBackend>();
	const FVulkanGraphicsAdapter& VulkanGraphicsAdapter = GraphicsAdapter->As<FVulkanGraphicsAdapter>();

	VkPhysicalDeviceFeatures EnabledFeatures = {};
	EnabledFeatures.samplerAnisotropy = VK_TRUE;

	const TVector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	const TSet<uint32> QueueFamilyIndices = {
		VulkanGraphicsAdapter.GetQueueMetadata(EGraphicsQueueType::Graphics).FamilyIndex,
		VulkanGraphicsAdapter.GetQueueMetadata(EGraphicsQueueType::Present).FamilyIndex,
		VulkanGraphicsAdapter.GetQueueMetadata(EGraphicsQueueType::Compute).FamilyIndex,
		VulkanGraphicsAdapter.GetQueueMetadata(EGraphicsQueueType::Transfer).FamilyIndex,
	};

	uint32 Index = 0;
	constexpr float32 QueuePriorities[] = {1.0f, 1.0f, 1.0f, 1.0f};
	TVector<VkDeviceQueueCreateInfo> QueueCreateInfos(QueueFamilyIndices.size());
	for (const uint32 QueueFamilyIndex : QueueFamilyIndices)
	{
		QueueCreateInfos[Index] = Vulkan::CreateStructure<VkDeviceQueueCreateInfo>();
		QueueCreateInfos[Index].queueFamilyIndex = QueueFamilyIndex;
		QueueCreateInfos[Index].queueCount = VulkanGraphicsAdapter.GetQueueCountForIndex(QueueFamilyIndex);
		QueueCreateInfos[Index].pQueuePriorities = QueuePriorities;
		++Index;
	}

	VkDeviceCreateInfo DeviceCreateInfo = Vulkan::CreateStructure<VkDeviceCreateInfo>();
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(DeviceExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.pEnabledFeatures = &EnabledFeatures;

	const VkPhysicalDevice PhysicalDevice = VulkanGraphicsAdapter.GetHandle();
	const VkResult Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, VulkanBackend.GetVulkanAllocator(), &Device);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan device");

	RW_LOG(LogGraphics, Debug, "Created vulkan device for adapter '{}'", GraphicsAdapter->GetProperties().Name);
}

void FVulkanGraphicsDevice::Destroy()
{
	const FVulkanGraphicsBackend& VulkanBackend = Backend->As<FVulkanGraphicsBackend>();
	vkDestroyDevice(Device, VulkanBackend.GetVulkanAllocator());
}
