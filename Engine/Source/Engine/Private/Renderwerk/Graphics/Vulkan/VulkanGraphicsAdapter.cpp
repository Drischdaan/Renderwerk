#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsAdapter.h"

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

void FVulkanGraphicsAdapter::Initialize()
{
	VkPhysicalDeviceProperties DeviceProperties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

	Properties.Name = DeviceProperties.deviceName;
	Properties.DeviceId = DeviceProperties.deviceID;
	Properties.Type = MapAdapterType(DeviceProperties.deviceType);
	Properties.Vendor = MapAdapterVendor(DeviceProperties.vendorID);
	Properties.DriverVersion = DeviceProperties.driverVersion;

	ApiVersion = DeviceProperties.apiVersion;
}

void FVulkanGraphicsAdapter::Destroy()
{
}
