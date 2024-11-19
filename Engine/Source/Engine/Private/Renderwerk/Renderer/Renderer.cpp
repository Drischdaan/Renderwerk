#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	ApiVersion = VK_API_VERSION_1_3;
	CreateInstance();
	CreateSurface();

	const TVector<TSharedPtr<FAdapter>> Adapters = QueryAdapters();
	RW_LOG(LogRenderer, Info, "Found {} adapters:", Adapters.size());
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
	{
		RW_LOG(LogRenderer, Info, "\tAdapter{}:", Adapter->GetIndex());
		RW_LOG(LogRenderer, Info, "\t\tName: {}", Adapter->GetName());
		RW_LOG(LogRenderer, Info, "\t\tType: {}", ToString(Adapter->GetType()));
		RW_LOG(LogRenderer, Info, "\t\tVendor: {}", ToString(Adapter->GetVendor()));
	}
	const TSharedPtr<FAdapter> Adapter = ChooseAdapter(Adapters);
	ASSERT_MSG(Adapter != nullptr, "No suitable adapter found");
	RW_LOG(LogRenderer, Info, "Chosen Adapter{}: {}", Adapter->GetIndex(), Adapter->GetName());
}

FRenderer::~FRenderer()
{
	vkDestroySurfaceKHR(Instance, Surface, Allocator);
	vkDestroyInstance(Instance, Allocator);
}

void FRenderer::CreateInstance()
{
	VkApplicationInfo ApplicationInfo = {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pApplicationName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.pEngineName = RW_ENGINE_NAME;
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;

	const TVector<const char*> InstanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};
	CheckExtensions(InstanceExtensions);

	InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(InstanceExtensions.size());

#ifdef RW_ENABLE_GPU_DEBUGGING
	const TVector<const char*> InstanceLayers = {
		"VK_LAYER_KHRONOS_validation",
	};
	CheckLayers(InstanceLayers);

	InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.data();
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(InstanceLayers.size());
#endif

	const VkResult Result = vkCreateInstance(&InstanceCreateInfo, Allocator, &Instance);
	ASSERT_MSG(Result == VK_SUCCESS, "Failed to create Vulkan instance");
}

void FRenderer::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = Description.Window->GetHandle();
	const VkResult Result = vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, Allocator, &Surface);
	ASSERT_MSG(Result == VK_SUCCESS, "Failed to create Vulkan surface");
}

TVector<TSharedPtr<FAdapter>> FRenderer::QueryAdapters() const
{
	uint32 AdapterCount = 0;
	vkEnumeratePhysicalDevices(Instance, &AdapterCount, nullptr);
	TVector<VkPhysicalDevice> PhysicalDevices(AdapterCount);
	vkEnumeratePhysicalDevices(Instance, &AdapterCount, PhysicalDevices.data());

	TVector<TSharedPtr<FAdapter>> Adapters;
	for (uint32 Index = 0; Index < AdapterCount; ++Index)
		Adapters.emplace_back(MakeShared<FAdapter>(Index, PhysicalDevices.at(Index), Surface));
	return Adapters;
}

TSharedPtr<FAdapter> FRenderer::ChooseAdapter(const TVector<TSharedPtr<FAdapter>>& Adapters) const
{
	for (TSharedPtr<FAdapter> Adapter : Adapters)
	{
		if (Adapter->GetType() != EAdapterType::Discrete)
			continue;
		if (Adapter->GetApiVersion() < ApiVersion)
			continue;
		if (!Adapter->SupportsExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME))
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME))
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_RAY_QUERY_EXTENSION_NAME))
			continue;
		return Adapter;
	}
	return nullptr;
}

void FRenderer::CheckExtensions(const TVector<const char*>& Extensions)
{
	uint32 ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, AvailableExtensions.data());
	for (const char* Extension : Extensions)
	{
		bool bFound = false;
		for (const VkExtensionProperties& AvailableExtension : AvailableExtensions)
		{
			if (strcmp(Extension, AvailableExtension.extensionName) == 0)
			{
				bFound = true;
				break;
			}
		}
		ASSERT_MSG(bFound, "Required extension not found");
	}
}

void FRenderer::CheckLayers(const TVector<const char*>& Layers)
{
	uint32 LayerCount = 0;
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	TVector<VkLayerProperties> AvailableLayers(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());
	for (const char* Layer : Layers)
	{
		bool bFound = false;
		for (const VkLayerProperties& AvailableLayer : AvailableLayers)
		{
			if (strcmp(Layer, AvailableLayer.layerName) == 0)
			{
				bFound = true;
				break;
			}
		}
		ASSERT_MSG(bFound, "Required layer not found");
	}
}
