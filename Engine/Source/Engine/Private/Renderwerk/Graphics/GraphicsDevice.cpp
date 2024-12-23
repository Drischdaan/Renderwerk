﻿#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsContext.h"

FGraphicsDevice::FGraphicsDevice() = default;

FGraphicsDevice::~FGraphicsDevice() = default;

void FGraphicsDevice::Initialize(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsAdapter>& InGraphicsAdapter)
{
	GraphicsContext = InGraphicsContext;
	GraphicsAdapter = InGraphicsAdapter;
	CreateDevice();
	AcquireQueues();
}

void FGraphicsDevice::Destroy() const
{
	vkDestroyDevice(Device, GraphicsContext->GetAllocator());
}

void FGraphicsDevice::WaitForIdle() const
{
	const FVulkanResult Result = vkDeviceWaitIdle(Device);
	ASSERT(Result == VK_SUCCESS, "Failed to wait for device to become idle");
}

void FGraphicsDevice::CreateDevice()
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
		QueueCreateInfos[Index] = {};
		QueueCreateInfos[Index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfos[Index].pNext = nullptr;
		QueueCreateInfos[Index].flags = 0;
		QueueCreateInfos[Index].queueFamilyIndex = QueueFamilyIndex;
		QueueCreateInfos[Index].queueCount = GraphicsAdapter->GetQueueCountForIndex(QueueFamilyIndex);
		QueueCreateInfos[Index].pQueuePriorities = QueuePriorities;
		++Index;
	}

	const TVector<const char*> RequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	};

	RW_LOG(LogGraphics, Info, "Enabled device extensions:");
	for (const char* RequiredExtension : RequiredExtensions)
	{
		RW_LOG(LogGraphics, Info, "\t- {}", RequiredExtension);
		ASSERT(GraphicsAdapter->SupportsExtension(RequiredExtension), "Required device extension '{}' is not supported", RequiredExtension);
	}

	VkPhysicalDeviceFeatures EnabledFeatures = {};
	EnabledFeatures.samplerAnisotropy = VK_TRUE;
	EnabledFeatures.fillModeNonSolid = VK_TRUE;

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddressFeatures = {};
	BufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddressFeatures.pNext = nullptr;
	BufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceExtendedDynamicState3FeaturesEXT ExtendedDynamicState3Features = {};
	ExtendedDynamicState3Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
	ExtendedDynamicState3Features.pNext = &BufferDeviceAddressFeatures;
	ExtendedDynamicState3Features.extendedDynamicState3PolygonMode = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
	DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	DynamicRenderingFeatures.pNext = &ExtendedDynamicState3Features;
	DynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features Synchronization2Features = {};
	Synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	Synchronization2Features.pNext = &DynamicRenderingFeatures;
	Synchronization2Features.synchronization2 = VK_TRUE;

	VkDeviceCreateInfo DeviceCreateInfo;
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = &Synchronization2Features;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = nullptr;
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();
	DeviceCreateInfo.pEnabledFeatures = &EnabledFeatures;

	const FVulkanResult Result = vkCreateDevice(GraphicsAdapter->GetHandle(), &DeviceCreateInfo, GraphicsContext->GetAllocator(), &Device);
	ASSERT(Result == VK_SUCCESS, "Failed to create device");
	volkLoadDevice(Device);
}

void FGraphicsDevice::AcquireQueues()
{
	GraphicsQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Graphics));
	PresentQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Present));
	ComputeQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Compute));
	TransferQueue = GetQueue(GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Transfer));
}

VkQueue FGraphicsDevice::GetQueue(const FGraphicsQueueMetadata& Metadata) const
{
	VkQueue Queue;
	vkGetDeviceQueue(Device, Metadata.FamilyIndex, Metadata.QueueIndex, &Queue);
	return Queue;
}
