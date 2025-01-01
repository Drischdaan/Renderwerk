#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsAdapter.h"
#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"

FVulkanGraphicsBackend::FVulkanGraphicsBackend()
	: IGraphicsBackend(EGraphicsBackendType::Vulkan)
{
}

FVulkanGraphicsBackend::~FVulkanGraphicsBackend() = default;

void FVulkanGraphicsBackend::Initialize(const FGraphicsBackendDesc& InDescription)
{
	Description = InDescription;

	VkResult Result = volkInitialize();
	ASSERT(Result == VK_SUCCESS, "Failed to initialize Vulkan loader");

	VulkanAllocator = nullptr;

	VkApplicationInfo ApplicationInfo = Vulkan::CreateStructure<VkApplicationInfo>();
	ApplicationInfo.pApplicationName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.pEngineName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.apiVersion = VK_API_VERSION_1_3;

	TVector<const char*> EnabledLayers = {};
	if (Description.bEnableGpuValidation)
		EnabledLayers.push_back("VK_LAYER_KHRONOS_validation");

	TVector<const char*> EnabledExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};
	if (Description.bEnableDebugging)
		EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	VkInstanceCreateInfo InstanceCreateInfo = Vulkan::CreateStructure<VkInstanceCreateInfo>();
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(EnabledLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = EnabledLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(EnabledExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.data();

	Result = vkCreateInstance(&InstanceCreateInfo, VulkanAllocator, &VulkanInstance);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan instance");
	volkLoadInstance(VulkanInstance);

	RW_LOG(LogGraphics, Info, "Vulkan backend initialized");
}

void FVulkanGraphicsBackend::Destroy()
{
	vkDestroyInstance(VulkanInstance, VulkanAllocator);
	volkFinalize();
}

TVector<TSharedPtr<IGraphicsAdapter>> FVulkanGraphicsBackend::GetAvailableAdapters()
{
	uint32 AdapterCount = 0;
	VkResult Result = vkEnumeratePhysicalDevices(VulkanInstance, &AdapterCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");
	TVector<VkPhysicalDevice> PhysicalDevices(AdapterCount);
	Result = vkEnumeratePhysicalDevices(VulkanInstance, &AdapterCount, PhysicalDevices.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");

	TVector<TSharedPtr<IGraphicsAdapter>> Adapters;
	for (VkPhysicalDevice PhysicalDevice : PhysicalDevices)
	{
		TSharedPtr<FVulkanGraphicsAdapter> Adapter = MakeShared<FVulkanGraphicsAdapter>(this, PhysicalDevice);
		Adapter->Initialize();
		Adapters.push_back(Adapter);
	}
	return Adapters;
}
