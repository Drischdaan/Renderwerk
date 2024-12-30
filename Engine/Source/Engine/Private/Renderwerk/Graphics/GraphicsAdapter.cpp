#include "pch.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

#include "Renderwerk/Graphics/VulkanUtility.h"

namespace
{
	EGraphicsAdapterVendor MapAdapterVendor(const uint32& InVendorID)
	{
		switch (InVendorID)
		{
		case static_cast<uint32>(EGraphicsAdapterVendor::NVIDIA):
			return EGraphicsAdapterVendor::NVIDIA;
		case static_cast<uint32>(EGraphicsAdapterVendor::AMD):
			return EGraphicsAdapterVendor::AMD;
		case static_cast<uint32>(EGraphicsAdapterVendor::Intel):
			return EGraphicsAdapterVendor::Intel;
		case static_cast<uint32>(EGraphicsAdapterVendor::Microsoft):
			return EGraphicsAdapterVendor::Microsoft;
		default:
			return EGraphicsAdapterVendor::Unknown;
		}
	}
}

FString GetVendorString(const EGraphicsAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EGraphicsAdapterVendor::NVIDIA: return "NVIDIA";
	case EGraphicsAdapterVendor::AMD: return "AMD";
	case EGraphicsAdapterVendor::Intel: return "Intel";
	case EGraphicsAdapterVendor::Microsoft: return "Microsoft";
	case EGraphicsAdapterVendor::Unknown:
	default:
		return "Unknown";
	}
}

FGraphicsAdapter::FGraphicsAdapter(const TSharedPtr<FGraphicsContext>& InContext, const VkPhysicalDevice InPhysicalDevice)
	: Context(InContext), PhysicalDevice(InPhysicalDevice)
{
}

FGraphicsAdapter::~FGraphicsAdapter() = default;

void FGraphicsAdapter::Initialize(const VkSurfaceKHR Surface)
{
	VkPhysicalDeviceProperties DeviceProperties = {};
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

	Properties.Name = DeviceProperties.deviceName;
	Properties.Type = static_cast<EGraphicsAdapterType>(DeviceProperties.deviceType);
	Properties.Vendor = MapAdapterVendor(DeviceProperties.vendorID);
	Properties.ApiVersion = DeviceProperties.apiVersion;
	Properties.DriverVersion = DeviceProperties.driverVersion;

	Layers = VulkanUtility::GetPhysicalDeviceLayers(PhysicalDevice);
	Extensions = VulkanUtility::GetPhysicalDeviceExtensions(PhysicalDevice);

	const TVector<VkQueueFamilyProperties> QueueFamilyProperties = VulkanUtility::GetQueueFamilyProperties(PhysicalDevice);
	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < static_cast<uint32>(QueueFamilyProperties.size()); ++Index)
	{
		uint8 CurrentTransferScore = 0;
		const VkQueueFamilyProperties QueueFamily = QueueFamilyProperties[Index];
		if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && !QueueMetadata.contains(EGraphicsQueueType::Graphics))
		{
			FGraphicsQueueMetadata GraphicsQueueMetadata = {};
			GraphicsQueueMetadata.FamilyIndex = Index;
			GraphicsQueueMetadata.QueueIndex = CurrentTransferScore;
			QueueMetadata.insert_or_assign(EGraphicsQueueType::Graphics, GraphicsQueueMetadata);
			++CurrentTransferScore;
			if (!QueueMetadata.contains(EGraphicsQueueType::Present))
			{
				VkBool32 SupportsPresent = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent);
				if (SupportsPresent)
				{
					FGraphicsQueueMetadata PresentQueueMetadata = {};
					PresentQueueMetadata.FamilyIndex = Index;
					PresentQueueMetadata.QueueIndex = CurrentTransferScore;
					QueueMetadata.insert_or_assign(EGraphicsQueueType::Present, PresentQueueMetadata);
					++CurrentTransferScore;
				}
			}
		}
		if (QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			FGraphicsQueueMetadata ComputeQueueMetadata = {};
			ComputeQueueMetadata.FamilyIndex = Index;
			ComputeQueueMetadata.QueueIndex = CurrentTransferScore;
			QueueMetadata.insert_or_assign(EGraphicsQueueType::Compute, ComputeQueueMetadata);
			++CurrentTransferScore;
		}
		if (QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (CurrentTransferScore <= BestTransferScore)
			{
				FGraphicsQueueMetadata TransferQueueMetadata = {};
				TransferQueueMetadata.FamilyIndex = Index;
				TransferQueueMetadata.QueueIndex = CurrentTransferScore;
				QueueMetadata.insert_or_assign(EGraphicsQueueType::Transfer, TransferQueueMetadata);
				BestTransferScore = CurrentTransferScore;
			}
		}
	}
}

bool8 FGraphicsAdapter::IsLayerSupported(FStringView LayerName) const
{
	return std::ranges::any_of(Layers, [LayerName](const FString& Layer)
	{
		return LayerName == Layer;
	});
}

bool8 FGraphicsAdapter::IsExtensionSupported(FStringView ExtensionName) const
{
	return std::ranges::any_of(Extensions, [ExtensionName](const FString& Extension)
	{
		return ExtensionName == Extension;
	});
}

FString FGraphicsAdapter::GetDriverVersionString() const
{
	FString DriverVersionString;
	switch (Properties.Vendor)
	{
	case EGraphicsAdapterVendor::NVIDIA:
		DriverVersionString = std::format("{}.{}", (Properties.DriverVersion >> 22) & 0x3ff, (Properties.DriverVersion >> 14) & 0x0ff);
		break;
	case EGraphicsAdapterVendor::Intel:
		DriverVersionString = std::format("{}.{}", Properties.DriverVersion >> 14, Properties.DriverVersion & 0x3fff);
		break;
	case EGraphicsAdapterVendor::Microsoft:
	case EGraphicsAdapterVendor::AMD:
	case EGraphicsAdapterVendor::Unknown:
	default:
		DriverVersionString = std::format("{}.{}.{}", VK_API_VERSION_MAJOR(Properties.DriverVersion), VK_API_VERSION_MAJOR(Properties.DriverVersion),
		                                  VK_API_VERSION_MAJOR(Properties.DriverVersion));
		break;
	}
	return DriverVersionString;
}

FGraphicsQueueMetadata FGraphicsAdapter::GetQueueMetadata(const EGraphicsQueueType QueueType) const
{
	VERIFY(QueueMetadata.contains(QueueType), "Queue metadata not found");
	return QueueMetadata.at(QueueType);
}

uint32 FGraphicsAdapter::GetQueueCountForIndex(uint32 QueueFamilyIndex) const
{
	return static_cast<uint32>(std::ranges::count_if(QueueMetadata, [QueueFamilyIndex](const auto& Metadata)
	{
		return Metadata.second.FamilyIndex == QueueFamilyIndex;
	}));
}

FGraphicsSurfaceCapabilities FGraphicsAdapter::GetSurfaceCapabilities(const VkSurfaceKHR Surface) const
{
	FGraphicsSurfaceCapabilities SurfaceCapabilities = {};
	const VkResult Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceCapabilities.Capabilities);
	VERIFY(Result == VK_SUCCESS, "Failed to get surface capabilities");
	SurfaceCapabilities.Formats = VulkanUtility::GetSurfaceFormats(PhysicalDevice, Surface);
	SurfaceCapabilities.PresentModes = VulkanUtility::GetPresentModes(PhysicalDevice, Surface);
	return SurfaceCapabilities;
}
