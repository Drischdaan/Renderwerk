#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"

#include "Renderwerk/Graphics/GfxFwd.hpp"
#include "Renderwerk/Graphics/GfxObject.hpp"

#include <directx/d3d12.h>
#include <directx/d3d12sdklayers.h>
#include <directx/d3d12shader.h>
#include <directx/d3dx12.h>

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <wrl.h>

template <typename T>
using TComPtr = Microsoft::WRL::ComPtr<T>;

class ENGINE_API IGfxContextChild : public IGfxObject
{
public:
	explicit IGfxContextChild(FGfxContext* InGfxContext);
	IGfxContextChild(FGfxContext* InGfxContext, const FStringView& InDebugName);
	virtual ~IGfxContextChild() override = default;

	NON_COPY_MOVEABLE(IGfxContextChild)

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] FGfxContext* GetGfxContext() const { return GfxContext; }

protected:
	FGfxContext* GfxContext;
};

class ENGINE_API IGfxAdapterChild : public IGfxObject
{
public:
	explicit IGfxAdapterChild(FGfxAdapter* InGfxAdapter);
	IGfxAdapterChild(FGfxAdapter* InGfxAdapter, const FStringView& InDebugName);
	virtual ~IGfxAdapterChild() override = default;

	NON_COPY_MOVEABLE(IGfxAdapterChild)

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] FGfxAdapter* GetGfxAdapter() const { return GfxAdapter; }

protected:
	FGfxAdapter* GfxAdapter;
};

class ENGINE_API IGfxDeviceChild : public IGfxObject
{
public:
	explicit IGfxDeviceChild(FGfxDevice* InGfxDevice);
	IGfxDeviceChild(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	virtual ~IGfxDeviceChild() override = default;

	NON_COPY_MOVEABLE(IGfxDeviceChild)

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] FGfxDevice* GetGfxDevice() const { return GfxDevice; }

protected:
	FGfxDevice* GfxDevice;
};

namespace D3DUtility
{
	[[nodiscard]] constexpr FStringView ToString(const D3D_FEATURE_LEVEL FeatureLevel)
	{
		switch (FeatureLevel)
		{
		case D3D_FEATURE_LEVEL_1_0_GENERIC: return TEXT("D3D_FEATURE_LEVEL_1_0_GENERIC");
		case D3D_FEATURE_LEVEL_1_0_CORE: return TEXT("D3D_FEATURE_LEVEL_1_0_CORE");
		case D3D_FEATURE_LEVEL_9_1: return TEXT("D3D_FEATURE_LEVEL_9_1");
		case D3D_FEATURE_LEVEL_9_2: return TEXT("D3D_FEATURE_LEVEL_9_2");
		case D3D_FEATURE_LEVEL_9_3: return TEXT("D3D_FEATURE_LEVEL_9_3");
		case D3D_FEATURE_LEVEL_10_0: return TEXT("D3D_FEATURE_LEVEL_10_0");
		case D3D_FEATURE_LEVEL_10_1: return TEXT("D3D_FEATURE_LEVEL_10_1");
		case D3D_FEATURE_LEVEL_11_0: return TEXT("D3D_FEATURE_LEVEL_11_0");
		case D3D_FEATURE_LEVEL_11_1: return TEXT("D3D_FEATURE_LEVEL_11_1");
		case D3D_FEATURE_LEVEL_12_0: return TEXT("D3D_FEATURE_LEVEL_12_0");
		case D3D_FEATURE_LEVEL_12_1: return TEXT("D3D_FEATURE_LEVEL_12_1");
		case D3D_FEATURE_LEVEL_12_2: return TEXT("D3D_FEATURE_LEVEL_12_2");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D_SHADER_MODEL ShaderModel)
	{
		switch (ShaderModel)
		{
		case D3D_SHADER_MODEL_NONE: return TEXT("D3D_SHADER_MODEL_NONE");
		case D3D_SHADER_MODEL_5_1: return TEXT("D3D_SHADER_MODEL_5_1");
		case D3D_SHADER_MODEL_6_0: return TEXT("D3D_SHADER_MODEL_6_0");
		case D3D_SHADER_MODEL_6_1: return TEXT("D3D_SHADER_MODEL_6_1");
		case D3D_SHADER_MODEL_6_2: return TEXT("D3D_SHADER_MODEL_6_2");
		case D3D_SHADER_MODEL_6_3: return TEXT("D3D_SHADER_MODEL_6_3");
		case D3D_SHADER_MODEL_6_4: return TEXT("D3D_SHADER_MODEL_6_4");
		case D3D_SHADER_MODEL_6_5: return TEXT("D3D_SHADER_MODEL_6_5");
		case D3D_SHADER_MODEL_6_6: return TEXT("D3D_SHADER_MODEL_6_6");
		case D3D_SHADER_MODEL_6_7: return TEXT("D3D_SHADER_MODEL_6_7");
		case D3D_SHADER_MODEL_6_8: return TEXT("D3D_SHADER_MODEL_6_8");
		case D3D_SHADER_MODEL_6_9: return TEXT("D3D_SHADER_MODEL_6_9");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_RESOURCE_BINDING_TIER ResourceBindingTier)
	{
		switch (ResourceBindingTier)
		{
		case D3D12_RESOURCE_BINDING_TIER_1: return TEXT("D3D12_RESOURCE_BINDING_TIER_1");
		case D3D12_RESOURCE_BINDING_TIER_2: return TEXT("D3D12_RESOURCE_BINDING_TIER_2");
		case D3D12_RESOURCE_BINDING_TIER_3: return TEXT("D3D12_RESOURCE_BINDING_TIER_3");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_RAYTRACING_TIER RaytracingTier)
	{
		switch (RaytracingTier)
		{
		case D3D12_RAYTRACING_TIER_NOT_SUPPORTED: return TEXT("D3D12_RAYTRACING_TIER_NOT_SUPPORTED");
		case D3D12_RAYTRACING_TIER_1_0: return TEXT("D3D12_RAYTRACING_TIER_1_0");
		case D3D12_RAYTRACING_TIER_1_1: return TEXT("D3D12_RAYTRACING_TIER_1_1");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_MESH_SHADER_TIER MeshShaderTier)
	{
		switch (MeshShaderTier)
		{
		case D3D12_MESH_SHADER_TIER_NOT_SUPPORTED: return TEXT("D3D12_MESH_SHADER_TIER_NOT_SUPPORTED");
		case D3D12_MESH_SHADER_TIER_1: return TEXT("D3D12_MESH_SHADER_TIER_1");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
	{
		switch (DescriptorHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER");
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_RTV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_DSV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_VARIABLE_SHADING_RATE_TIER VariableShadingRateTier)
	{
		switch (VariableShadingRateTier)
		{
		case D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED: return TEXT("D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED");
		case D3D12_VARIABLE_SHADING_RATE_TIER_1: return TEXT("D3D12_VARIABLE_SHADING_RATE_TIER_1");
		case D3D12_VARIABLE_SHADING_RATE_TIER_2: return TEXT("D3D12_VARIABLE_SHADING_RATE_TIER_2");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_WORK_GRAPHS_TIER WorkGraphsTier)
	{
		switch (WorkGraphsTier)
		{
		case D3D12_WORK_GRAPHS_TIER_NOT_SUPPORTED: return TEXT("D3D12_WORK_GRAPHS_TIER_NOT_SUPPORTED");
		case D3D12_WORK_GRAPHS_TIER_1_0: return TEXT("D3D12_WORK_GRAPHS_TIER_1_0");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion)
	{
		switch (RootSignatureVersion)
		{
		case D3D_ROOT_SIGNATURE_VERSION_1_0: return TEXT("D3D_ROOT_SIGNATURE_VERSION_1_0");
		case D3D_ROOT_SIGNATURE_VERSION_1_1: return TEXT("D3D_ROOT_SIGNATURE_VERSION_1_1");
		case D3D_ROOT_SIGNATURE_VERSION_1_2: return TEXT("D3D_ROOT_SIGNATURE_VERSION_1_2");
		}
		return TEXT("Unknown");
	}
}
