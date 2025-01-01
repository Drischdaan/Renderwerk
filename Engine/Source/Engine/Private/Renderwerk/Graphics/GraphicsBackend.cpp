#include "pch.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsBackend.h"
#include "Renderwerk/Graphics/None/NoneGraphicsBackend.h"
#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"

TSharedPtr<IGraphicsAdapter> IGraphicsBackend::GetAdapterByDeviceId(const uint32 DeviceId)
{
	const TVector<TSharedPtr<IGraphicsAdapter>> GraphicsAdapters = GetAvailableAdapters();
	for (const TSharedPtr<IGraphicsAdapter>& GraphicsAdapter : GraphicsAdapters)
	{
		if (GraphicsAdapter->GetProperties().DeviceId == DeviceId)
		{
			return GraphicsAdapter;
		}
	}
	return nullptr;
}

TSharedPtr<IGraphicsAdapter> IGraphicsBackend::GetSuitableAdapter(const TSharedPtr<IGraphicsWindowContext>& WindowContext)
{
	const TVector<TSharedPtr<IGraphicsAdapter>> GraphicsAdapters = GetAvailableAdapters();
	for (const TSharedPtr<IGraphicsAdapter>& GraphicsAdapter : GraphicsAdapters)
	{
		GraphicsAdapter->Initialize(WindowContext);
		if (IsAdapterSuitable(GraphicsAdapter))
			return GraphicsAdapter;
	}
	return nullptr;
}

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
