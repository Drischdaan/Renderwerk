#include "pch.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

namespace
{
	EGraphicsAdapterType MapAdapterType(const VkPhysicalDeviceType& InType)
	{
		switch (InType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return EGraphicsAdapterType::Discrete;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return EGraphicsAdapterType::Integrated;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return EGraphicsAdapterType::Virtual;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return EGraphicsAdapterType::Software;
		default:
			return EGraphicsAdapterType::None;
		}
	}

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

FGraphicsAdapter::FGraphicsAdapter() = default;

FGraphicsAdapter::~FGraphicsAdapter() = default;

void FGraphicsAdapter::Initialize(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const VkPhysicalDevice& InPhysicalDevice, const VkSurfaceKHR& Surface)
{
	GraphicsContext = InGraphicsContext;
	PhysicalDevice = InPhysicalDevice;
	AcquireProperties();
	AcquireExtensionsAndLayers();
	AcquireQueueMetadata(Surface);
}

bool8 FGraphicsAdapter::SupportsExtension(const char* ExtensionName)
{
	return std::ranges::any_of(SupportedExtensions, [ExtensionName](const VkExtensionProperties& Extension)
	{
		return strcmp(Extension.extensionName, ExtensionName) == 0;
	});
}

bool8 FGraphicsAdapter::SupportsLayer(const char* LayerName)
{
	return std::ranges::any_of(SupportedLayers, [LayerName](const VkLayerProperties& Layer)
	{
		return strcmp(Layer.layerName, LayerName) == 0;
	});
}

FGraphicsQueueMetadata FGraphicsAdapter::GetQueueMetadata(const EGraphicsQueueType QueueType) const
{
	return QueueMetadata.at(QueueType);
}

uint32 FGraphicsAdapter::GetQueueCountForIndex(const uint32 QueueFamilyIndex) const
{
	return static_cast<uint32>(std::ranges::count_if(QueueMetadata, [QueueFamilyIndex](const auto& Metadata)
	{
		return Metadata.second.FamilyIndex == QueueFamilyIndex;
	}));
}

FGraphicsSurfaceProperties FGraphicsAdapter::GetSurfaceProperties(const VkSurfaceKHR& Surface) const
{
	FGraphicsSurfaceProperties SurfaceProperties = {};
	FVulkanResult Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceProperties.Capabilities);
	ASSERT(Result == VK_SUCCESS, "Failed to get surface capabilities");

	uint32 FormatCount = 0;
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get surface formats");
	SurfaceProperties.Formats.resize(FormatCount);
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, SurfaceProperties.Formats.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get surface formats");

	uint32 PresentModeCount = 0;
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get present modes");
	SurfaceProperties.PresentModes.resize(PresentModeCount);
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, SurfaceProperties.PresentModes.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get present modes");
	return SurfaceProperties;
}

FString FGraphicsAdapter::GetDriverVersionString() const
{
	FString DriverVersionString;
	switch (Vendor)
	{
	case EGraphicsAdapterVendor::NVIDIA:
		DriverVersionString = std::format("{}.{}", (DriverVersion >> 22) & 0x3ff, (DriverVersion >> 14) & 0x0ff);
		break;
	case EGraphicsAdapterVendor::Intel:
		DriverVersionString = std::format("{}.{}", DriverVersion >> 14, DriverVersion & 0x3fff);
		break;
	case EGraphicsAdapterVendor::Microsoft:
	case EGraphicsAdapterVendor::AMD:
	case EGraphicsAdapterVendor::Unknown:
	default:
		DriverVersionString = std::format("{}.{}.{}", VK_API_VERSION_MAJOR(DriverVersion), VK_API_VERSION_MAJOR(DriverVersion), VK_API_VERSION_MAJOR(DriverVersion));
		break;
	}
	return DriverVersionString;
}

void FGraphicsAdapter::AcquireProperties()
{
	VkPhysicalDeviceProperties Properties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
	Name = Properties.deviceName;
	Type = MapAdapterType(Properties.deviceType);
	Vendor = MapAdapterVendor(Properties.vendorID);
	DriverVersion = Properties.driverVersion;
	ApiVersion = Properties.apiVersion;
}

void FGraphicsAdapter::AcquireExtensionsAndLayers()
{
	uint32 ExtensionCount = 0;
	FVulkanResult Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate device extension properties");
	SupportedExtensions.resize(ExtensionCount);
	Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, SupportedExtensions.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate device extension properties");

	uint32 LayerCount = 0;
	Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate device layer properties");
	SupportedLayers.resize(LayerCount);
	Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, SupportedLayers.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate device layer properties");
}

void FGraphicsAdapter::AcquireQueueMetadata(const VkSurfaceKHR& Surface)
{
	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	constexpr uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
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
			}
		}
	}

	RW_LOG(LogGraphics, Info, "|   Type   | Family Index | Queue Index |");
	RW_LOG(LogGraphics, Info, "|----------|--------------|-------------|");
	for (const auto& [Type, Metadata] : QueueMetadata)
	{
		FString QueueName = FString(GetEnumValueName(Type));
		const size64 Padding = 8 - QueueName.length();
		if (Padding > 0)
		{
			for (size64 Index = 0; Index < Padding; ++Index)
				QueueName += " ";
		}
		RW_LOG(LogGraphics, Info, "| {} |       {}      |      {}      |", QueueName, Metadata.FamilyIndex, Metadata.QueueIndex);
	}
}
