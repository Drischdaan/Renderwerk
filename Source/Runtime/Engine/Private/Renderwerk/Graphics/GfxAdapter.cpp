#include "pch.hpp"

#include "Renderwerk/Graphics/GfxAdapter.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"

FGfxAdapter::FGfxAdapter(FGfxContext* InGfxContext, const TComPtr<IDXGIAdapter4>& InAdapter)
	: FGfxAdapter(InGfxContext, InAdapter, TEXT("UnnamedAdapter"))
{
}

FGfxAdapter::FGfxAdapter(FGfxContext* InGfxContext, const TComPtr<IDXGIAdapter4>& InAdapter, const FStringView& InDebugName)
	: IGfxContextChild(InGfxContext, InDebugName), Adapter(InAdapter)
{
	HRESULT Result = Adapter->GetDesc3(&AdapterDesc);
	RW_VERIFY_ID(Result);

	Type = EGfxAdapterType::Discrete;
	if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
	{
		Type = EGfxAdapterType::Software;
	}
	else if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_REMOTE)
	{
		Type = EGfxAdapterType::Remote;
	}

	TComPtr<ID3D12Device> TempDevice;
	Result = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
	RW_VERIFY_ID(Result);

	FeatureSupport.Init(TempDevice.Get());
}

FGfxAdapter::~FGfxAdapter()
{
	Adapter.Reset();
}

TRef<FGfxDevice> FGfxAdapter::CreateDevice(const FGfxDeviceDesc& DeviceDesc, const FStringView& DebugName)
{
	return NewRef<FGfxDevice>(this, DeviceDesc, DebugName);
}

FGfxAdapterMemoryStats FGfxAdapter::QueryMemoryStats(const EGfxAdapterMemoryType MemoryType) const
{
	DXGI_MEMORY_SEGMENT_GROUP SegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_LOCAL;
	switch (MemoryType)
	{
	case EGfxAdapterMemoryType::Dedicated:
		SegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_LOCAL;
		break;
	case EGfxAdapterMemoryType::Shared:
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

void FGfxAdapter::PrintAdapterInfo() const
{
	RW_LOG(Info, "Adapter '{}' Info:", GetName());
	RW_LOG(Info, "{{");
	RW_LOG(Info, "\t\"Vendor\": {}", GetVendorString(GetVendor()));
	RW_LOG(Info, "\t\"VideoMemory\": {}", FStringUtilities::FormatMemorySize(GetDedicatedVideoMemory()));
	RW_LOG(Info, "\t\"SystemMemory\": {}", FStringUtilities::FormatMemorySize(GetSharedSystemMemory()));
	RW_LOG(Info, "\t\"Features\":");
	RW_LOG(Info, "\t{{");
	RW_LOG(Info, "\t\t\"Level\": {}", D3DUtility::ToString(GetFeatureLevel()));
	RW_LOG(Info, "\t\t\"ShaderModel\": {}", D3DUtility::ToString(GetShaderModel()));
	RW_LOG(Info, "\t\t\"ResourceBinding\": {}", D3DUtility::ToString(GetResourceBindingTier()));
	RW_LOG(Info, "\t\t\"Raytracing\": {}", D3DUtility::ToString(GetRaytracingTier()));
	RW_LOG(Info, "\t\t\"MeshShader\": {}", D3DUtility::ToString(GetMeshShaderTier()));
	RW_LOG(Info, "\t\t\"VariableRateShading\": {}", D3DUtility::ToString(GetVariableShadingRateTier()));
	RW_LOG(Info, "\t\t\"WorkGraphs\": {}", D3DUtility::ToString(GetWorkGraphsTier()));
	RW_LOG(Info, "\t\t\"RootSignatureVersion\": {}", D3DUtility::ToString(GetRootSignatureVersion()));
	RW_LOG(Info, "\t\t\"EnhancedBarriers\": {}", IsSupportingEnhancedBarriers());
	RW_LOG(Info, "\t}}");
	RW_LOG(Info, "}}");
}

EGfxAdapterType FGfxAdapter::GetType() const
{
	return Type;
}

EGfxAdapterVendor FGfxAdapter::GetVendor() const
{
	return ConvertVendorId(AdapterDesc.VendorId);
}

size64 FGfxAdapter::GetDedicatedVideoMemory() const
{
	return AdapterDesc.DedicatedVideoMemory;
}

size64 FGfxAdapter::GetSharedSystemMemory() const
{
	return AdapterDesc.SharedSystemMemory;
}

FStringView FGfxAdapter::GetName() const
{
	return AdapterDesc.Description;
}

D3D_FEATURE_LEVEL FGfxAdapter::GetFeatureLevel() const
{
	return FeatureSupport.MaxSupportedFeatureLevel();
}

D3D_SHADER_MODEL FGfxAdapter::GetShaderModel() const
{
	return FeatureSupport.HighestShaderModel();
}

D3D12_RESOURCE_BINDING_TIER FGfxAdapter::GetResourceBindingTier() const
{
	return FeatureSupport.ResourceBindingTier();
}

D3D12_RAYTRACING_TIER FGfxAdapter::GetRaytracingTier() const
{
	return FeatureSupport.RaytracingTier();
}

D3D12_MESH_SHADER_TIER FGfxAdapter::GetMeshShaderTier() const
{
	return FeatureSupport.MeshShaderTier();
}

D3D12_VARIABLE_SHADING_RATE_TIER FGfxAdapter::GetVariableShadingRateTier() const
{
	return FeatureSupport.VariableShadingRateTier();
}

D3D12_WORK_GRAPHS_TIER FGfxAdapter::GetWorkGraphsTier() const
{
	return FeatureSupport.WorkGraphsTier();
}

D3D_ROOT_SIGNATURE_VERSION FGfxAdapter::GetRootSignatureVersion() const
{
	return FeatureSupport.HighestRootSignatureVersion();
}

bool8 FGfxAdapter::IsSupportingEnhancedBarriers() const
{
	return FeatureSupport.EnhancedBarriersSupported();
}

FNativeObject FGfxAdapter::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::DXGI_Adapter)
	{
		return Adapter.Get();
	}
	return IGfxContextChild::GetRawNativeObject(NativeObjectId);
}
