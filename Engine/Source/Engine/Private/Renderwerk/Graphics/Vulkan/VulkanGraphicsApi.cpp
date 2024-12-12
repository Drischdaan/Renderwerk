#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsApi.h"

#include "Renderwerk/Graphics/Vulkan/VulkanTypes.h"

TMap<void*, FVulkanAllocation> FVulkanAllocator::AllocationMap;
TMap<VkSystemAllocationScope, size64> FVulkanAllocator::UsagePerScope;

void* FVulkanAllocator::Allocate([[maybe_unused]] void* UserData, const size64 Size, const size64 Alignment, const VkSystemAllocationScope Scope)
{
	ASSERT(Size > 0, "Size must be greater than zero");
	void* Pointer = FMemory::Allocate(Size, Alignment);
	AllocationMap[Pointer] = {Size, Scope};
	UsagePerScope[Scope] += Size;
	return Pointer;
}

void* FVulkanAllocator::Reallocate([[maybe_unused]] void* UserData, void* OriginalPointer, const size64 Size, const size64 Alignment, const VkSystemAllocationScope Scope)
{
	ASSERT(OriginalPointer, "Original pointer is null");
	ASSERT(AllocationMap.contains(OriginalPointer), "Pointer not found in allocation map");
	void* Pointer = FMemory::Reallocate(OriginalPointer, Size, Alignment);
	AllocationMap[Pointer] = {Size, Scope};
	UsagePerScope[Scope] += Size;
	return Pointer;
}

void FVulkanAllocator::Free([[maybe_unused]] void* UserData, void* Pointer)
{
	if (!Pointer) return;
	ASSERT(AllocationMap.contains(Pointer), "Pointer not found in allocation map");
	const FVulkanAllocation Allocation = AllocationMap.at(Pointer);
	UsagePerScope[Allocation.Scope] -= Allocation.Size;
	AllocationMap.erase(Pointer);
	FMemory::Free(Pointer);
}

FVulkanGraphicsApi::FVulkanGraphicsApi()
	: IGraphicsApi(EGraphicsApiType::Vulkan)
{
	VulkanAllocator = new VkAllocationCallbacks();
	VulkanAllocator->pfnAllocation = FVulkanAllocator::Allocate;
	VulkanAllocator->pfnReallocation = FVulkanAllocator::Reallocate;
	VulkanAllocator->pfnFree = FVulkanAllocator::Free;

	CreateInstance();
}

FVulkanGraphicsApi::~FVulkanGraphicsApi()
{
	vkDestroyInstance(Instance, VulkanAllocator);
}

void FVulkanGraphicsApi::CreateInstance()
{
	uint32 InstanceVersion = VK_API_VERSION_1_0;
	const VkResult VersionResult = vkEnumerateInstanceVersion(&InstanceVersion);
	VERIFY(VersionResult == VK_SUCCESS, "Failed to get Vulkan instance version");

	VERIFY(VK_API_VERSION_MAJOR(InstanceVersion) >= VK_API_VERSION_MAJOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan instance version is too low");
	VERIFY(VK_API_VERSION_MINOR(InstanceVersion) >= VK_API_VERSION_MINOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan instance version is too low");

	VkApplicationInfo ApplicationInfo = {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pApplicationName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.pEngineName = RW_ENGINE_NAME;
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.apiVersion = InstanceVersion;

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = nullptr;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;

	// Naively assume that the extensions are available
	// TODO: Check if the extensions are available
	const TVector<FVulkanExtensionId> InstanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};

	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(InstanceExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

	const FVulkanResult Result = vkCreateInstance(&InstanceCreateInfo, VulkanAllocator, &Instance);
	VERIFY(Result, "Failed to create Vulkan instance");
}
