#include "pch.h"

#include "Renderwerk/RHI/D3D12/D3D12Adapter.h"

EAdapterVendor ConvertVendor(const uint32 VendorId)
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (VendorId)
	{
	DEFINE_CASE(NVIDIA);
	DEFINE_CASE(AMD);
	DEFINE_CASE(Intel);
	DEFINE_CASE(Microsoft);
	default:
		return EAdapterVendor::None;
	}
#undef DEFINE_CASE
}

FD3D12Adapter::FD3D12Adapter(const uint32& InIndex, const TComPtr<IDXGIFactory7>& InFactory, const TComPtr<IDXGIAdapter4>& InAdapter)
	: IAdapter(InIndex), Factory(InFactory), Adapter(InAdapter)
{
	DXGI_ADAPTER_DESC3 AdapterDescription;
	D3D_CHECKM(Adapter->GetDesc3(&AdapterDescription), "Failed to get adapter description")

	Name = AdapterDescription.Description;
	if (AdapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || AdapterDescription.VendorId & DXGI_ADAPTER_FLAG_REMOTE)
		Type = EAdapterType::Software;
	else
		Type = EAdapterType::Discrete;
	Vendor = ConvertVendor(AdapterDescription.VendorId);

	TComPtr<ID3D12Device> TempDevice;
	FD3D12Result CreateResult = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
	D3D_CHECKM(CreateResult, "Failed to create device! Device doesn't support feature level 11.0")

	CD3DX12FeatureSupport FeatureSupport;
	FD3D12Result FeatureInitResult = FeatureSupport.Init(TempDevice.Get());
	D3D_CHECKM(FeatureInitResult, "Failed to initialize feature support!")

	FeatureLevel = FeatureSupport.MaxSupportedFeatureLevel();
	ShaderModel = FeatureSupport.HighestShaderModel();

	Capabilities.AvailableVideoMemory = AdapterDescription.DedicatedVideoMemory;

	if (FeatureSupport.RaytracingTier() != D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
		Capabilities.SupportedFeatures.push_back({EAdapterFeatureType::Raytracing, static_cast<uint32>(FeatureSupport.RaytracingTier())});
	if (FeatureSupport.VariableShadingRateTier() != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED)
		Capabilities.SupportedFeatures.push_back({EAdapterFeatureType::VariableRateShading, static_cast<uint32>(FeatureSupport.VariableShadingRateTier())});
	if (FeatureSupport.MeshShaderTier() != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
		Capabilities.SupportedFeatures.push_back({EAdapterFeatureType::MeshShading, static_cast<uint32>(FeatureSupport.MeshShaderTier())});

	TempDevice.Reset();
}

FD3D12Adapter::~FD3D12Adapter()
{
	Adapter.Reset();
}
