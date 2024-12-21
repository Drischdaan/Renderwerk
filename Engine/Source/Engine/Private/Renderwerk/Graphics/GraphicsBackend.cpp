#include "pch.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

#include "Renderwerk/Graphics/VulkanUtility.h"

FGraphicsBackend::FGraphicsBackend() = default;

FGraphicsBackend::~FGraphicsBackend() = default;

void FGraphicsBackend::Initialize(const FGraphicsBackendDesc& InDescription)
{
	Description = InDescription;

	const VkResult Result = volkInitialize();
	ASSERT(Result == VK_SUCCESS, "Failed to initialize volk");

	TVector<const char*> RequiredLayers = {
#if RW_ENABLE_GRAPHICS_VALIDATION
		"VK_LAYER_KHRONOS_validation"
#endif
	};
	RequiredLayers.insert(RequiredLayers.end(), Description.AdditionalLayers.begin(), Description.AdditionalLayers.end());

	TVector<const char*> RequiredExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#if RW_ENABLE_GRAPHICS_VALIDATION
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
	};
	RequiredExtensions.insert(RequiredExtensions.end(), Description.AdditionalExtensions.begin(), Description.AdditionalExtensions.end());

	InstanceProperties = QueryInstanceProperties();
	RW_LOG(LogGraphics, Info, "Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(InstanceProperties.ApiVersion), VK_VERSION_MINOR(InstanceProperties.ApiVersion),
	       VK_VERSION_PATCH(InstanceProperties.ApiVersion));
	Instance = CreateInstance(InstanceProperties, RequiredLayers, RequiredExtensions);
	volkLoadInstance(Instance);
}

void FGraphicsBackend::Destroy() const
{
	vkDestroyInstance(Instance, nullptr);
	volkFinalize();
}

FGraphicsBackend::FInstanceProperties FGraphicsBackend::QueryInstanceProperties()
{
	FInstanceProperties Properties = {};
	Properties.ApiVersion = VulkanUtility::GetInstanceVersion();
	Properties.Layers = VulkanUtility::GetInstanceLayers();
	Properties.Extensions = VulkanUtility::GetInstanceExtensions();
	return Properties;
}

void FGraphicsBackend::ValidateRequiredInstanceObjects(FString ObjectName, TSpan<FString> AvailableObjects, const TSpan<const char*> RequiredObjects)
{
	for (FStringView RequiredObject : RequiredObjects)
	{
		const bool8 bWasObjectFound = std::ranges::any_of(AvailableObjects, [&](const FStringView AvailableObject)
		{
			return AvailableObject == RequiredObject;
		});
		ASSERT(bWasObjectFound, "Required Vulkan {} '{}' not found", ObjectName, RequiredObject);
	}
}

VkInstance FGraphicsBackend::CreateInstance(FInstanceProperties& InProperties, const TSpan<const char*> RequiredLayers, const TSpan<const char*> RequiredExtensions)
{
	ValidateRequiredInstanceObjects("layer", InProperties.Layers, RequiredLayers);
	ValidateRequiredInstanceObjects("extension", InProperties.Extensions, RequiredExtensions);

	VkApplicationInfo ApplicationInfo = Vulkan::CreateStructure<VkApplicationInfo>();
	ApplicationInfo.pApplicationName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.pEngineName = "Renderwerk";
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo InstanceCreateInfo = Vulkan::CreateStructure<VkInstanceCreateInfo>();
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(RequiredLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = RequiredLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();

	VkInstance Instance = VK_NULL_HANDLE;
	const VkResult Result = vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan instance");
	RW_LOG(LogGraphics, Trace, "Created Vulkan instance");
	return Instance;
}
