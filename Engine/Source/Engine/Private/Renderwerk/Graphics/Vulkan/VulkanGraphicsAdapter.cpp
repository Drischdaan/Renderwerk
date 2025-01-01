#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsAdapter.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsWindowContext.h"

namespace
{
	EGraphicsAdapterType MapAdapterType(const VkPhysicalDeviceType InType)
	{
		switch (InType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			return EGraphicsAdapterType::Unknown;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return EGraphicsAdapterType::Integrated;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return EGraphicsAdapterType::Discrete;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return EGraphicsAdapterType::Virtual;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return EGraphicsAdapterType::Software;
		default:
			return EGraphicsAdapterType::Unknown;
		}
	}
}

FVulkanGraphicsAdapter::FVulkanGraphicsAdapter(IGraphicsBackend* InBackend, const VkPhysicalDevice InPhysicalDevice)
	: IGraphicsAdapter(InBackend), PhysicalDevice(InPhysicalDevice)
{
}

FVulkanGraphicsAdapter::~FVulkanGraphicsAdapter() = default;

void FVulkanGraphicsAdapter::Initialize(const TSharedPtr<IGraphicsWindowContext>& InGraphicsWindowContext)
{
	GraphicsWindowContext = InGraphicsWindowContext;

	VkPhysicalDeviceProperties DeviceProperties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

	Properties.Name = DeviceProperties.deviceName;
	Properties.DeviceId = DeviceProperties.deviceID;
	Properties.Type = MapAdapterType(DeviceProperties.deviceType);
	Properties.Vendor = MapAdapterVendor(DeviceProperties.vendorID);
	Properties.DriverVersion = DeviceProperties.driverVersion;

	ApiVersion = DeviceProperties.apiVersion;

	uint32 LayerCount = 0;
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	TVector<VkLayerProperties> LayerProperties(LayerCount);
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, LayerProperties.data());
	Layers.reserve(LayerCount);
	for (const VkLayerProperties& LayerProperty : LayerProperties)
		Layers.emplace_back(LayerProperty.layerName);

	uint32 ExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> ExtensionProperties(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, ExtensionProperties.data());
	Extensions.reserve(ExtensionCount);
	for (const VkExtensionProperties& ExtensionProperty : ExtensionProperties)
		Extensions.emplace_back(ExtensionProperty.extensionName);

	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	const FVulkanGraphicsWindowContext& VulkanGraphicsWindowContext = GraphicsWindowContext->As<FVulkanGraphicsWindowContext>();
	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < static_cast<uint32>(QueueFamilyProperties.size()); ++Index)
	{
		uint8 CurrentTransferScore = 0;
		const VkQueueFamilyProperties QueueFamily = QueueFamilyProperties[Index];
		if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && !QueueMetadataMap.contains(EGraphicsQueueType::Graphics))
		{
			FVulkanQueueMetadata GraphicsQueueMetadata = {};
			GraphicsQueueMetadata.FamilyIndex = Index;
			GraphicsQueueMetadata.QueueIndex = CurrentTransferScore;
			QueueMetadataMap.insert_or_assign(EGraphicsQueueType::Graphics, GraphicsQueueMetadata);
			++CurrentTransferScore;
			if (!QueueMetadataMap.contains(EGraphicsQueueType::Present))
			{
				VkBool32 SupportsPresent = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, VulkanGraphicsWindowContext.GetSurface(), &SupportsPresent);
				if (SupportsPresent)
				{
					FVulkanQueueMetadata PresentQueueMetadata = {};
					PresentQueueMetadata.FamilyIndex = Index;
					PresentQueueMetadata.QueueIndex = CurrentTransferScore;
					QueueMetadataMap.insert_or_assign(EGraphicsQueueType::Present, PresentQueueMetadata);
					++CurrentTransferScore;
				}
			}
		}
		if (QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			FVulkanQueueMetadata ComputeQueueMetadata = {};
			ComputeQueueMetadata.FamilyIndex = Index;
			ComputeQueueMetadata.QueueIndex = CurrentTransferScore;
			QueueMetadataMap.insert_or_assign(EGraphicsQueueType::Compute, ComputeQueueMetadata);
			++CurrentTransferScore;
		}
		if (QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (CurrentTransferScore <= BestTransferScore)
			{
				FVulkanQueueMetadata TransferQueueMetadata = {};
				TransferQueueMetadata.FamilyIndex = Index;
				TransferQueueMetadata.QueueIndex = CurrentTransferScore;
				QueueMetadataMap.insert_or_assign(EGraphicsQueueType::Transfer, TransferQueueMetadata);
				BestTransferScore = CurrentTransferScore;
			}
		}
	}
}

void FVulkanGraphicsAdapter::Destroy()
{
	GraphicsWindowContext.reset();
}

bool8 FVulkanGraphicsAdapter::SupportsLayer(const FString& LayerName) const
{
	return std::ranges::any_of(Layers, [&LayerName](const FString& Layer) { return Layer == LayerName; });
}

bool8 FVulkanGraphicsAdapter::SupportsExtension(const FString& ExtensionName) const
{
	return std::ranges::any_of(Extensions, [&ExtensionName](const FString& Extension) { return Extension == ExtensionName; });
}

FVulkanQueueMetadata FVulkanGraphicsAdapter::GetQueueMetadata(const EGraphicsQueueType Type) const
{
	return QueueMetadataMap.at(Type);
}

uint32 FVulkanGraphicsAdapter::GetQueueCountForIndex(uint32 QueueFamilyIndex) const
{
	return static_cast<uint32>(std::ranges::count_if(QueueMetadataMap, [QueueFamilyIndex](const auto& Metadata)
	{
		return Metadata.second.FamilyIndex == QueueFamilyIndex;
	}));
}
