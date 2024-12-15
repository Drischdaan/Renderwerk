#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <vulkan/vulkan_core.h>

struct ENGINE_API FVulkanResult
{
public:
	constexpr FVulkanResult() = default;

	constexpr FVulkanResult(const VkResult InResult)
		: Result(InResult)
	{
	}

	constexpr ~FVulkanResult() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanResult);

public:
	[[nodiscard]] constexpr bool8 IsSuccess() const
	{
		return Result == VK_SUCCESS;
	}

	[[nodiscard]] constexpr bool8 IsError() const
	{
		return !IsSuccess();
	}

public:
	constexpr operator bool8() const { return IsSuccess(); }
	constexpr bool8 operator!() const { return IsError(); }

	constexpr operator VkResult() const { return Result; }
	constexpr operator const VkResult() const { return Result; }

	constexpr bool8 operator==(const FVulkanResult& Other) const { return Result == Other.Result; }
	constexpr bool8 operator!=(const FVulkanResult& Other) const { return Result != Other.Result; }
	constexpr bool8 operator==(const VkResult& Other) const { return Result == Other; }
	constexpr bool8 operator!=(const VkResult& Other) const { return Result != Other; }

private:
	VkResult Result = VK_SUCCESS;
};

using FGraphicsFrameId = uint64;

struct ENGINE_API FGraphicsFrame
{
	VkCommandPool CommandPool = VK_NULL_HANDLE;
	TSharedPtr<FVulkanCommandBuffer> CommandBuffer = nullptr;
	VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
	VkFence InFlightFence = VK_NULL_HANDLE;
};
