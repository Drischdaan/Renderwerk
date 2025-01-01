#include "pch.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

#include "Renderwerk/Graphics/None/NoneGraphicsBackend.h"
#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"

TUniquePtr<IGraphicsBackend> IGraphicsBackend::Create(const EGraphicsBackendType InBackendType)
{
	switch (InBackendType)
	{
	case EGraphicsBackendType::Vulkan: return MakeUnique<FVulkanGraphicsBackend>();
	case EGraphicsBackendType::DirectX12:
	case EGraphicsBackendType::None:
	default:
		return MakeUnique<FNoneGraphicsBackend>();
	}
}
