#include "pch.hpp"

#include "Renderwerk/Renderer/GraphicsContext.hpp"

FGraphicsContext::FGraphicsContext(const FGraphicsContextDesc& InDescription)
	: Description(InDescription)
{
	if (Description.bEnableDebugLayer)
	{
		HRESULT Result = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug));
		RW_VERIFY_ID(Result);
		DXGIDebug->EnableLeakTrackingForThread();

		Result = D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug));
		RW_VERIFY_ID(Result);
		D3D12Debug->SetEnableAutoName(true);
		D3D12Debug->EnableDebugLayer();
		if (Description.bEnableGPUValidation)
		{
			D3D12Debug->SetEnableGPUBasedValidation(true);
			RW_LOG(Warning, "GPU Validation is activated. This may impact performance");
		}
	}

	const uint32 FactoryFlags = Description.bEnableDebugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
	const HRESULT Result = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory));
	RW_VERIFY_ID(Result);
}

FGraphicsContext::~FGraphicsContext()
{
	Factory.Reset();
	if (Description.bEnableDebugLayer)
	{
		D3D12Debug.Reset();
		if (DXGIDebug)
		{
			const HRESULT Result = DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
			RW_VERIFY_ID(Result);
		}
	}
}

TObjectHandle<FAdapter> FGraphicsContext::GetSuitableAdapter()
{
	TObjectHandle<IDXGIAdapter4> TempAdapter;
	for (uint32 Index = 0; Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) != DXGI_ERROR_NOT_FOUND; ++Index)
	{
		TObjectHandle<FAdapter> Adapter = NewObjectHandle<FAdapter>(this, TempAdapter);
		if (Adapter->GetType() != EAdapterType::Discrete
			|| Adapter->GetFeatureLevel() < D3D_FEATURE_LEVEL_12_0
			|| Adapter->GetShaderModel() < D3D_SHADER_MODEL_6_7
			|| Adapter->GetResourceBindingTier() < D3D12_RESOURCE_BINDING_TIER_3
			|| Adapter->GetRaytracingTier() == D3D12_RAYTRACING_TIER_NOT_SUPPORTED
			|| Adapter->GetMeshShaderTier() == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
		{
			RW_LOG(Info, "Adapter '{}' doesn't meet requirements. Skipping...", Adapter->GetName());
			continue;
		}
		return Adapter;
	}
	return nullptr;
}
