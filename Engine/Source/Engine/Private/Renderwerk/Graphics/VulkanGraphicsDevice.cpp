#include "pch.h"

#include "Renderwerk/Graphics/VulkanGraphicsDevice.h"

#include "Renderwerk/Graphics/VulkanGraphicsAdapter.h"

FVulkanGraphicsDevice::FVulkanGraphicsDevice(const FVulkanContext& InContext, const TSharedPtr<FVulkanGraphicsAdapter>& InAdapter)
	: Context(InContext), Adapter(InAdapter)
{
	CreateDevice();
	AcquireQueues();
}

FVulkanGraphicsDevice::~FVulkanGraphicsDevice()
{
	vkDestroyDevice(Device, Context.Allocator);
	Adapter.reset();
}

void FVulkanGraphicsDevice::WaitForIdle() const
{
	vkDeviceWaitIdle(Device);
}

void FVulkanGraphicsDevice::CreateDevice()
{
	VERIFY(Adapter->GetSupported13Features().dynamicRendering, "Adapter does not support dynamic rendering");
	VERIFY(Adapter->GetSupported13Features().synchronization2, "Adapter does not support synchronization2");
	VERIFY(Adapter->GetSupported12Features().bufferDeviceAddress, "Adapter does not support buffer device address");
	VERIFY(Adapter->GetSupported12Features().descriptorIndexing, "Adapter does not support descriptor indexing");
	VERIFY(Adapter->GetSupportedDeviceFeatures().samplerAnisotropy, "Adapter does not support sampler anisotropy");

	const FVulkanQueueMetadata QueueMetadata = Adapter->GetQueueMetadata();

	const TSet<uint32> UniqueQueueFamilies = {QueueMetadata.GraphicsIndex, QueueMetadata.ComputeIndex, QueueMetadata.TransferIndex, QueueMetadata.PresentIndex};
	const bool8 bIsGraphicsSharingPresent = QueueMetadata.GraphicsIndex == QueueMetadata.PresentIndex;

	constexpr float32 QueuePriorities[] = {1.0f, 0.9f};
	TVector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	QueueCreateInfos.reserve(UniqueQueueFamilies.size());
	for (const uint32 QueueIndex : UniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo QueueCreateInfo = {};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.pNext = nullptr;
		QueueCreateInfo.flags = 0;
		QueueCreateInfo.queueFamilyIndex = QueueIndex;
		QueueCreateInfo.pQueuePriorities = QueuePriorities;
		QueueCreateInfo.queueCount = 1;
		if (QueueIndex == QueueMetadata.GraphicsIndex && bIsGraphicsSharingPresent)
			QueueCreateInfo.queueCount = 2;
		QueueCreateInfos.push_back(QueueCreateInfo);
	}

	const TVector<const char*> RequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	};

	for (const char* RequiredExtension : RequiredExtensions)
	{
		VERIFY(Adapter->SupportsExtension(RequiredExtension), "Adapter does not support required extension: {}", RequiredExtension);
	}

	VkPhysicalDeviceFeatures EnabledFeatures = {};
	EnabledFeatures.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeaturesKHR DynamicRenderingFeatures = {};
	DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	DynamicRenderingFeatures.pNext = nullptr;
	DynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2FeaturesKHR Synchronization2Features = {};
	Synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
	Synchronization2Features.pNext = &DynamicRenderingFeatures;
	Synchronization2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceBufferDeviceAddressFeaturesKHR BufferDeviceAddressFeatures = {};
	BufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
	BufferDeviceAddressFeatures.pNext = &Synchronization2Features;
	BufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeaturesEXT DescriptorIndexingFeatures = {};
	DescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	DescriptorIndexingFeatures.pNext = &BufferDeviceAddressFeatures;
	DescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = &DescriptorIndexingFeatures;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = nullptr;
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();
	DeviceCreateInfo.pEnabledFeatures = &EnabledFeatures;

	const FVulkanResult Result = vkCreateDevice(Adapter->PhysicalDevice, &DeviceCreateInfo, Context.Allocator, &Device);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan device");
}

void FVulkanGraphicsDevice::AcquireQueues()
{
	const FVulkanQueueMetadata QueueMetadata = Adapter->GetQueueMetadata();
	const bool8 bIsGraphicsSharingPresent = QueueMetadata.GraphicsIndex == QueueMetadata.PresentIndex;

	vkGetDeviceQueue(Device, QueueMetadata.GraphicsIndex, 0, &GraphicsQueue);
	vkGetDeviceQueue(Device, QueueMetadata.ComputeIndex, 0, &ComputeQueue);
	vkGetDeviceQueue(Device, QueueMetadata.TransferIndex, 0, &TransferQueue);
	vkGetDeviceQueue(Device, QueueMetadata.PresentIndex, bIsGraphicsSharingPresent ? 1 : 0, &PresentQueue);
}
