#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

// Values taken from https://pcisig.com/membership/member-companies
enum class ENGINE_API EGfxAdapterVendor : uint16
{
	Unknown = 0,
	Microsoft = 0x1414,
	Intel = 0x8086,
	AMD = 0x1022,
	NVIDIA = 0x10DE,
};

enum class ENGINE_API EGfxAdapterType : uint8
{
	Discrete = 0,
	Software,
	Remote,
};

enum class ENGINE_API EGfxAdapterMemoryType : uint8
{
	Dedicated = 0,
	Shared,
};

struct ENGINE_API FGfxAdapterMemoryStats
{
	size64 AvailableMemory = 0;
	size64 UsedMemory = 0;
};

class ENGINE_API FGfxAdapter : public IGfxContextChild
{
public:
	explicit FGfxAdapter(FGfxContext* InGfxContext, const TComPtr<IDXGIAdapter4>& InAdapter);
	FGfxAdapter(FGfxContext* InGfxContext, const TComPtr<IDXGIAdapter4>& InAdapter, const FStringView& InDebugName);
	~FGfxAdapter() override;

	NON_COPY_MOVEABLE(FGfxAdapter)

public:
	[[nodiscard]] TRef<FGfxDevice> CreateDevice(const FGfxDeviceDesc& DeviceDesc, const FStringView& DebugName = TEXT("UnnamedDevice"));

	[[nodiscard]] FGfxAdapterMemoryStats QueryMemoryStats(EGfxAdapterMemoryType MemoryType = EGfxAdapterMemoryType::Dedicated) const;

	void PrintAdapterInfo() const;

public:
	[[nodiscard]] FStringView GetName() const;
	[[nodiscard]] EGfxAdapterType GetType() const;
	[[nodiscard]] EGfxAdapterVendor GetVendor() const;

	[[nodiscard]] size64 GetDedicatedVideoMemory() const;
	[[nodiscard]] size64 GetSharedSystemMemory() const;

	[[nodiscard]] D3D_FEATURE_LEVEL GetFeatureLevel() const;
	[[nodiscard]] D3D_SHADER_MODEL GetShaderModel() const;

	[[nodiscard]] D3D12_RESOURCE_BINDING_TIER GetResourceBindingTier() const;
	[[nodiscard]] D3D12_RAYTRACING_TIER GetRaytracingTier() const;
	[[nodiscard]] D3D12_MESH_SHADER_TIER GetMeshShaderTier() const;
	[[nodiscard]] D3D12_VARIABLE_SHADING_RATE_TIER GetVariableShadingRateTier() const;
	[[nodiscard]] D3D12_WORK_GRAPHS_TIER GetWorkGraphsTier() const;
	[[nodiscard]] D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion() const;
	[[nodiscard]] bool8 IsSupportingEnhancedBarriers() const;

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] static constexpr EGfxAdapterVendor ConvertVendorId(uint32 Value);
	[[nodiscard]] static constexpr uint32 ConvertVendor(EGfxAdapterVendor Vendor);
	[[nodiscard]] static constexpr FStringView GetVendorString(EGfxAdapterVendor Vendor);

private:
	TComPtr<IDXGIAdapter4> Adapter;

	DXGI_ADAPTER_DESC3 AdapterDesc;
	EGfxAdapterType Type;

	CD3DX12FeatureSupport FeatureSupport;
};

constexpr EGfxAdapterVendor FGfxAdapter::ConvertVendorId(const uint32 Value)
{
	switch (Value)
	{
	case static_cast<uint32>(EGfxAdapterVendor::Unknown): return EGfxAdapterVendor::Unknown;
	case static_cast<uint32>(EGfxAdapterVendor::Microsoft): return EGfxAdapterVendor::Microsoft;
	case static_cast<uint32>(EGfxAdapterVendor::Intel): return EGfxAdapterVendor::Intel;
	case static_cast<uint32>(EGfxAdapterVendor::AMD): return EGfxAdapterVendor::AMD;
	case static_cast<uint32>(EGfxAdapterVendor::NVIDIA): return EGfxAdapterVendor::NVIDIA;
	default:
		return EGfxAdapterVendor::Unknown;
	}
}

constexpr uint32 FGfxAdapter::ConvertVendor(const EGfxAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EGfxAdapterVendor::Unknown: return static_cast<uint32>(EGfxAdapterVendor::Unknown);
	case EGfxAdapterVendor::Microsoft: return static_cast<uint32>(EGfxAdapterVendor::Microsoft);
	case EGfxAdapterVendor::Intel: return static_cast<uint32>(EGfxAdapterVendor::Intel);
	case EGfxAdapterVendor::AMD: return static_cast<uint32>(EGfxAdapterVendor::AMD);
	case EGfxAdapterVendor::NVIDIA: return static_cast<uint32>(EGfxAdapterVendor::NVIDIA);
	}
	return 0;
}

constexpr FStringView FGfxAdapter::GetVendorString(const EGfxAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EGfxAdapterVendor::Unknown: return TEXT("Unknown");
	case EGfxAdapterVendor::Microsoft: return TEXT("Microsoft");
	case EGfxAdapterVendor::Intel: return TEXT("Intel");
	case EGfxAdapterVendor::AMD: return TEXT("AMD");
	case EGfxAdapterVendor::NVIDIA: return TEXT("NVIDIA");
	}
	return TEXT("Unknown");
}
