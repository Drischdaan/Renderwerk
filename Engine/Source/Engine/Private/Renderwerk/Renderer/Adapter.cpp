#include "pch.h"

#include "Renderwerk/Renderer/Adapter.h"

EAdapterVendor ConvertVendor(const uint32 VendorID)
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (VendorID)
	{
	DEFINE_CASE(AMD);
	DEFINE_CASE(NVIDIA);
	DEFINE_CASE(Intel);
	default:
		return EAdapterVendor::Unknown;
	}
#undef DEFINE_CASE
}

FString ToString(const EAdapterType Type)
{
	switch (Type)
	{
	DEFINE_ENUM_CASE(EAdapterType, Unknown);
	DEFINE_ENUM_CASE(EAdapterType, Discrete);
	DEFINE_ENUM_CASE(EAdapterType, Integrated);
	default:
		return "Unknown";
	}
}

FString ToString(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	DEFINE_ENUM_CASE(EAdapterVendor, Unknown);
	DEFINE_ENUM_CASE(EAdapterVendor, AMD);
	DEFINE_ENUM_CASE(EAdapterVendor, NVIDIA);
	DEFINE_ENUM_CASE(EAdapterVendor, Intel);
	default:
		return "Unknown";
	}
}

FAdapter::FAdapter(const uint32 Index, const VkPhysicalDevice PhysicalDevice, const VkSurfaceKHR Surface)
	: Index(Index), PhysicalDevice(PhysicalDevice)
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
	Name = Properties.deviceName;
	if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		Type = EAdapterType::Discrete;
	else if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		Type = EAdapterType::Integrated;
	else
		Type = EAdapterType::Unknown;
	Vendor = ConvertVendor(Properties.vendorID);

	uint32 ExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, AvailableExtensions.data());
	for (VkExtensionProperties Extension : AvailableExtensions)
		Extensions.emplace_back(Extension.extensionName);

	uint32 LayerCount = 0;
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	TVector<VkLayerProperties> AvailableLayers(LayerCount);
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, AvailableLayers.data());
	for (VkLayerProperties Layer : AvailableLayers)
		Layers.emplace_back(Layer.layerName);

	QueryQueueInfos(Surface);
}

FAdapter::~FAdapter()
{
}

bool8 FAdapter::SupportsExtension(const FString& Extension) const
{
	return HasMatch(Extensions, Extension);
}

bool8 FAdapter::SupportsLayer(const FString& Layer) const
{
	return HasMatch(Layers, Layer);
}

FQueueInfo FAdapter::GetQueueInfo(const EQueueType Type) const
{
	return QueueInfos.at(Type);
}

void FAdapter::QueryQueueInfos(const VkSurfaceKHR Surface)
{
	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies.data());

	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
	{
		uint8 TransferScore = 0;
		const VkQueueFamilyProperties Properties = QueueFamilies.at(Index);
		FQueueInfo QueueInfo = {EQueueType::Graphics, Index, Properties};

		if (!QueueInfos.contains(EQueueType::Graphics) && Properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			QueueInfo.Type = EQueueType::Graphics;
			QueueInfos.emplace(QueueInfo.Type, QueueInfo);
			++TransferScore;
			if (!QueueInfos.contains(EQueueType::Present))
			{
				VkBool32 SupportsPresent = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent);
				if (SupportsPresent)
				{
					QueueInfo.Type = EQueueType::Present;
					QueueInfos.emplace(QueueInfo.Type, QueueInfo);
					++TransferScore;
				}
			}
		}

		if (Properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			QueueInfo.Type = EQueueType::Compute;
			QueueInfos.insert_or_assign(QueueInfo.Type, QueueInfo);
			++TransferScore;
		}

		if (Properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (TransferScore <= BestTransferScore)
			{
				BestTransferScore = TransferScore;
				QueueInfo.Type = EQueueType::Transfer;
				QueueInfos.insert_or_assign(QueueInfo.Type, QueueInfo);
			}
		}
	}
}

bool8 FAdapter::HasMatch(const TVector<FString>& Available, const FString& Required)
{
	for (const FString& Extension : Available)
	{
		if (Extension == Required)
			return true;
	}
	return false;
}
