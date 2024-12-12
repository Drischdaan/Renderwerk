#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"

#include <vulkan/vulkan_core.h>

DECLARE_LOG_CHANNEL(LogVulkan);

using FVulkanExtensionId = const FAnsiChar*;
using FVulkanLayerId = const FAnsiChar*;

ENGINE_API TVector<FVulkanExtensionId> MapFeatureTypesToExtensionsId(const TVector<EGraphicsFeatureType>& FeatureTypes);

struct ENGINE_API FVulkanResult
{
public:
	FVulkanResult() = default;
	FVulkanResult(VkResult InResult);
	~FVulkanResult() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanResult);

public:
	[[nodiscard]] bool8 IsSuccess() const;
	[[nodiscard]] bool8 IsError() const;

	[[nodiscard]] FString ToString() const;

public:
	[[nodiscard]] VkResult GetResult() const { return Result; }

public:
	operator bool8() const { return IsSuccess(); }
	operator const bool8() const { return IsSuccess(); }

	operator VkResult() const { return Result; }
	operator const VkResult() const { return Result; }

	bool8 operator!() const { return IsError(); }

	friend bool8 operator==(const FVulkanResult& Lhs, const FVulkanResult& Rhs) { return Lhs.Result == Rhs.Result; }
	friend bool8 operator!=(const FVulkanResult& Lhs, const FVulkanResult& Rhs) { return !(Lhs == Rhs); }

private:
	VkResult Result = VK_SUCCESS;
};
