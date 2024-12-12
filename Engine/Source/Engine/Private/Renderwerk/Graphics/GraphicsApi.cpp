#include "pch.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsApi.h"

IGraphicsApi::IGraphicsApi(const EGraphicsApiType InType)
	: Type(InType)
{
}

IGraphicsApi::~IGraphicsApi() = default;

TUniquePtr<IGraphicsApi> IGraphicsApi::NewInstance(const EGraphicsApiType Type)
{
	switch (Type)
	{
	case EGraphicsApiType::Vulkan: return MakeUnique<FVulkanGraphicsApi>();
	case EGraphicsApiType::None:
	default: break;
	}
	return nullptr;
}
