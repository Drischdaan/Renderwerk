#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Graphics/GraphicsApi.h"

#include <vulkan/vulkan.hpp>

// Define the required Vulkan instance version, based on needed features
#define REQUIRED_VULKAN_INSTANCE_VERSION VK_API_VERSION_1_3

struct ENGINE_API FVulkanAllocation
{
	size64 Size;
	VkSystemAllocationScope Scope;
};

struct ENGINE_API FVulkanAllocator
{
public:
	static void* Allocate(void* UserData, size64 Size, size64 Alignment, VkSystemAllocationScope Scope);
	static void* Reallocate(void* UserData, void* OriginalPointer, size64 Size, size64 Alignment, VkSystemAllocationScope Scope);
	static void Free(void* UserData, void* Pointer);

public:
	static TMap<void*, FVulkanAllocation> AllocationMap;
	static TMap<VkSystemAllocationScope, size64> UsagePerScope;
};

class ENGINE_API FVulkanGraphicsApi : public IGraphicsApi
{
public:
	FVulkanGraphicsApi();
	~FVulkanGraphicsApi() override;

	DELETE_COPY_AND_MOVE(FVulkanGraphicsApi);

private:
	void CreateInstance();

private:
	VkInstance Instance;
	VkAllocationCallbacks* VulkanAllocator = nullptr;
};
