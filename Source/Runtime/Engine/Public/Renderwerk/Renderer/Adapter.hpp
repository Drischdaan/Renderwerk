#pragma once

#include "Device.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FGraphicsContext;

// Values taken from https://pcisig.com/membership/member-companies
enum class ENGINE_API EAdapterVendor : uint16
{
	Unknown = 0,
	Microsoft = 0x1414,
	Intel = 0x8086,
	AMD = 0x1022,
	NVIDIA = 0x10DE,
};

enum class ENGINE_API EAdapterType : uint8
{
	Discrete = 0,
	Software,
	Remote,
};

enum class ENGINE_API EAdapterMemoryType : uint8
{
	Dedicated = 0,
	Shared,
};

struct ENGINE_API FAdapterMemoryStats
{
	size64 AvailableMemory = 0;
	size64 UsedMemory = 0;
};

class ENGINE_API FAdapter : public TGraphicsObject<FAdapter>
{
public:
	FAdapter(FGraphicsContext* InContext, const TObjectHandle<IDXGIAdapter4>& InAdapter);
	~FAdapter() override;

	NON_COPY_MOVEABLE(FAdapter)

public:
	[[nodiscard]] FAdapterMemoryStats QueryMemoryStats(EAdapterMemoryType MemoryType = EAdapterMemoryType::Dedicated) const;

	[[nodiscard]] TObjectHandle<FDevice> CreateDevice(const FDeviceDesc& DeviceDesc);

public:
	[[nodiscard]] FStringView GetName() const;
	[[nodiscard]] EAdapterType GetType() const;
	[[nodiscard]] EAdapterVendor GetVendor() const;

	[[nodiscard]] size64 GetDedicatedVideoMemory() const;
	[[nodiscard]] size64 GetSharedSystemMemory() const;

	[[nodiscard]] D3D_FEATURE_LEVEL GetFeatureLevel() const;
	[[nodiscard]] D3D_SHADER_MODEL GetShaderModel() const;

	[[nodiscard]] D3D12_RESOURCE_BINDING_TIER GetResourceBindingTier() const;
	[[nodiscard]] D3D12_RAYTRACING_TIER GetRaytracingTier() const;
	[[nodiscard]] D3D12_MESH_SHADER_TIER GetMeshShaderTier() const;

public:
	[[nodiscard]] TObjectHandle<IDXGIAdapter4> GetHandle() const { return Adapter; }

	[[nodiscard]] FGraphicsContext* GetContext() const { return Context; }

	[[nodiscard]] static constexpr EAdapterVendor ConvertVendorId(uint32 Value);
	[[nodiscard]] static constexpr uint32 ConvertVendor(EAdapterVendor Vendor);
	[[nodiscard]] static constexpr FStringView GetVendorString(EAdapterVendor Vendor);

private:
	FGraphicsContext* Context;
	TObjectHandle<IDXGIAdapter4> Adapter;

	DXGI_ADAPTER_DESC3 AdapterDesc;
	EAdapterType Type;

	CD3DX12FeatureSupport FeatureSupport;
};

constexpr EAdapterVendor FAdapter::ConvertVendorId(const uint32 Value)
{
	switch (Value)
	{
	case static_cast<uint32>(EAdapterVendor::Unknown): return EAdapterVendor::Unknown;
	case static_cast<uint32>(EAdapterVendor::Microsoft): return EAdapterVendor::Microsoft;
	case static_cast<uint32>(EAdapterVendor::Intel): return EAdapterVendor::Intel;
	case static_cast<uint32>(EAdapterVendor::AMD): return EAdapterVendor::AMD;
	case static_cast<uint32>(EAdapterVendor::NVIDIA): return EAdapterVendor::NVIDIA;
	default:
		return EAdapterVendor::Unknown;
	}
}

constexpr uint32 FAdapter::ConvertVendor(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EAdapterVendor::Unknown: return static_cast<uint32>(EAdapterVendor::Unknown);
	case EAdapterVendor::Microsoft: return static_cast<uint32>(EAdapterVendor::Microsoft);
	case EAdapterVendor::Intel: return static_cast<uint32>(EAdapterVendor::Intel);
	case EAdapterVendor::AMD: return static_cast<uint32>(EAdapterVendor::AMD);
	case EAdapterVendor::NVIDIA: return static_cast<uint32>(EAdapterVendor::NVIDIA);
	}
	return 0;
}

constexpr FStringView FAdapter::GetVendorString(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EAdapterVendor::Unknown: return TEXT("Unknown");
	case EAdapterVendor::Microsoft: return TEXT("Microsoft");
	case EAdapterVendor::Intel: return TEXT("Intel");
	case EAdapterVendor::AMD: return TEXT("AMD");
	case EAdapterVendor::NVIDIA: return TEXT("NVIDIA");
	}
	return TEXT("Unknown");
}
