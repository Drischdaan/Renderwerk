#include "pch.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsBackend.h"
#include "Renderwerk/Graphics/None/NoneGraphicsBackend.h"
#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"

TUniquePtr<IGraphicsBackend> IGraphicsBackend::Create(const EGraphicsBackendType InBackendType)
{
	switch (InBackendType)
	{
	case EGraphicsBackendType::Vulkan: return MakeUnique<FVulkanGraphicsBackend>();
	case EGraphicsBackendType::DirectX12: return MakeUnique<FDirectX12GraphicsBackend>();
	case EGraphicsBackendType::None:
	default:
		return MakeUnique<FNoneGraphicsBackend>();
	}
}
