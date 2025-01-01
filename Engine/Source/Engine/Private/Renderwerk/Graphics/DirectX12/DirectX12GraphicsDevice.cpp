#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsDevice.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsAdapter.h"

FDirectX12GraphicsDevice::FDirectX12GraphicsDevice(IGraphicsBackend* InBackend)
	: IGraphicsDevice(InBackend)
{
}

FDirectX12GraphicsDevice::~FDirectX12GraphicsDevice() = default;

void FDirectX12GraphicsDevice::Initialize(const TSharedPtr<IGraphicsAdapter>& InGraphicsAdapter)
{
	GraphicsAdapter = InGraphicsAdapter;

	const FDirectX12GraphicsAdapter& DirectX12GraphicsAdapter = static_cast<const FDirectX12GraphicsAdapter&>(*GraphicsAdapter);

	IDXGIAdapter4* Adapter = DirectX12GraphicsAdapter.GetHandle().Get();
	const D3D_FEATURE_LEVEL FeatureLevel = DirectX12GraphicsAdapter.GetFeatureSupport().MaxSupportedFeatureLevel();
	const HRESULT Result = D3D12CreateDevice(Adapter, FeatureLevel, IID_PPV_ARGS(Device.GetAddressOf()));
	ASSERT(SUCCEEDED(Result), "Failed to create D3D12 device");

	RW_LOG(LogGraphics, Debug, "Created directx12 device for adapter '{}'", GraphicsAdapter->GetProperties().Name);
}

void FDirectX12GraphicsDevice::Destroy()
{
	Device.Reset();
}
