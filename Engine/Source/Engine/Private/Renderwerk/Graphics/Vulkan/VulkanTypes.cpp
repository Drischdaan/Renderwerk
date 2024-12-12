#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanTypes.h"

#include "Renderwerk/Graphics/GraphicsTypes.h"

#include <vulkan/vulkan_core.h>

DEFINE_LOG_CHANNEL(LogVulkan);

TVector<FVulkanExtensionId> MapFeatureTypesToExtensionsId(const TVector<EGraphicsFeatureType>& FeatureTypes)
{
	TVector<FVulkanExtensionId> FeatureIds;
	for (const EGraphicsFeatureType FeatureType : FeatureTypes)
	{
		switch (FeatureType)
		{
		case EGraphicsFeatureType::MeshShaders:
			FeatureIds.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
			break;
		case EGraphicsFeatureType::RayTracing:
			FeatureIds.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
			FeatureIds.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
			FeatureIds.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
			break;
		case EGraphicsFeatureType::VariableRateShading:
			FeatureIds.push_back(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
			break;
		case EGraphicsFeatureType::None:
		default:
			break;
		}
	}
	return FeatureIds;
}

FVulkanResult::FVulkanResult(const VkResult InResult)
	: Result(InResult)
{
}

bool8 FVulkanResult::IsSuccess() const
{
	return Result == VK_SUCCESS;
}

bool8 FVulkanResult::IsError() const
{
	return Result != VK_SUCCESS;
}

FString FVulkanResult::ToString() const
{
	return FString(GetEnumValueName(Result));
}
