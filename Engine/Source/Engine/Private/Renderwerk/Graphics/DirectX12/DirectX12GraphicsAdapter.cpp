#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsAdapter.h"

#include "Renderwerk/Core/Utility/StringUtility.h"

FDirectX12GraphicsAdapter::FDirectX12GraphicsAdapter(IGraphicsBackend* GraphicsBackend, const ComPtr<IDXGIAdapter4>& Adapter)
	: IGraphicsAdapter(GraphicsBackend), Adapter(Adapter)
{
}

FDirectX12GraphicsAdapter::~FDirectX12GraphicsAdapter() = default;

void FDirectX12GraphicsAdapter::Initialize(const TSharedPtr<IGraphicsWindowContext>& InGraphicsWindowContext)
{
	GraphicsWindowContext = InGraphicsWindowContext;

	DXGI_ADAPTER_DESC3 AdapterDesc;
	const HRESULT Result = Adapter->GetDesc3(&AdapterDesc);
	ASSERT(SUCCEEDED(Result), "Failed to get adapter description");

	Properties.Name = StringUtility::ToNarrow(AdapterDesc.Description);
	Properties.DeviceId = AdapterDesc.DeviceId;
	Properties.Type = AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ? EGraphicsAdapterType::Software : EGraphicsAdapterType::Discrete;
	Properties.Vendor = MapAdapterVendor(AdapterDesc.VendorId);
	// DirectX 12 doesn't provide a driver version, so we need to use vendor specific APIs or the registry to get it
	Properties.DriverVersion = AdapterDesc.DeviceId;
}

void FDirectX12GraphicsAdapter::Destroy()
{
	GraphicsWindowContext.reset();
	Adapter.Reset();
}
