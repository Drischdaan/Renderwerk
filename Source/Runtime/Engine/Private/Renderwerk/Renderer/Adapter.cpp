#include "pch.hpp"

#include "Renderwerk/Renderer/Adapter.hpp"

FAdapter::FAdapter(FGraphicsContext* InContext, const TObjectHandle<IDXGIAdapter4>& InAdapter)
	: Context(InContext), Adapter(InAdapter)
{
	HRESULT Result = Adapter->GetDesc3(&AdapterDesc);
	RW_VERIFY_ID(Result);
	{
		TObjectHandle<ID3D12Device> TempDevice;
		Result = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
		RW_VERIFY_ID(Result);

		Type = EAdapterType::Discrete;
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		{
			Type = EAdapterType::Software;
		}
		else if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_REMOTE)
		{
			Type = EAdapterType::Remote;
		}

		FeatureSupport.Init(TempDevice);
	}
}

FAdapter::~FAdapter()
{
	Adapter.Reset();
}

FAdapterMemoryStats FAdapter::QueryMemoryStats(const EAdapterMemoryType MemoryType) const
{
	DXGI_MEMORY_SEGMENT_GROUP SegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_LOCAL;
	switch (MemoryType)
	{
	case EAdapterMemoryType::Dedicated:
		SegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_LOCAL;
		break;
	case EAdapterMemoryType::Shared:
		SegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL;
		break;
	}
	DXGI_QUERY_VIDEO_MEMORY_INFO VideoMemoryInfo = {};
	const HRESULT Result = Adapter->QueryVideoMemoryInfo(0, SegmentGroup, &VideoMemoryInfo);
	RW_VERIFY_ID(Result);
	return {
		.AvailableMemory = VideoMemoryInfo.Budget,
		.UsedMemory = VideoMemoryInfo.CurrentUsage,
	};
}

TObjectHandle<FDevice> FAdapter::CreateDevice(const FDeviceDesc& DeviceDesc)
{
	return NewObjectHandle<FDevice>(this, DeviceDesc);
}

FStringView FAdapter::GetName() const
{
	return AdapterDesc.Description;
}

EAdapterType FAdapter::GetType() const
{
	return Type;
}

EAdapterVendor FAdapter::GetVendor() const
{
	return ConvertVendorId(AdapterDesc.VendorId);
}

size64 FAdapter::GetDedicatedVideoMemory() const
{
	return AdapterDesc.DedicatedVideoMemory;
}

size64 FAdapter::GetSharedSystemMemory() const
{
	return AdapterDesc.SharedSystemMemory;
}

D3D_FEATURE_LEVEL FAdapter::GetFeatureLevel() const
{
	return FeatureSupport.MaxSupportedFeatureLevel();
}

D3D_SHADER_MODEL FAdapter::GetShaderModel() const
{
	return FeatureSupport.HighestShaderModel();
}

D3D12_RESOURCE_BINDING_TIER FAdapter::GetResourceBindingTier() const
{
	return FeatureSupport.ResourceBindingTier();
}

D3D12_RAYTRACING_TIER FAdapter::GetRaytracingTier() const
{
	return FeatureSupport.RaytracingTier();
}

D3D12_MESH_SHADER_TIER FAdapter::GetMeshShaderTier() const
{
	return FeatureSupport.MeshShaderTier();
}
