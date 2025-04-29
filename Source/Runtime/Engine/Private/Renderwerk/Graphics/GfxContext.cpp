#include "pch.hpp"

#include "Renderwerk/Graphics/GfxContext.hpp"

#include "Renderwerk/Graphics/GfxAdapter.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FGfxContext::FGfxContext(const FGfxContextDesc& InContextDesc)
	: ContextDesc(InContextDesc)
{
	uint32 FactoryFlags = 0;
	if (ContextDesc.bEnableDebugLayer)
	{
		const HRESULT Result = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug));
		RW_VERIFY_ID(Result);

		DXGIDebug->EnableLeakTrackingForThread();

		FactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}

	const HRESULT Result = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory));
	RW_VERIFY_ID(Result);
}

FGfxContext::~FGfxContext()
{
	Factory.Reset();
	if (ContextDesc.bEnableDebugLayer && DXGIDebug)
	{
		const HRESULT Result = DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		RW_VERIFY_ID(Result);
	}
	DXGIDebug.Reset();
}

TRef<FGfxAdapter> FGfxContext::GetSuitableAdapter()
{
	PROFILE_FUNCTION();

	uint8 BestAdapterScore = 0;
	uint32 BestAdapterIndex = 0;
	TComPtr<IDXGIAdapter4> TempAdapter;
	RW_LOG(Info, "Adapter Scores:");
	for (uint32 Index = 0;; ++Index)
	{
		if (Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		const uint8 AdapterScore = CalculateAdapterScore(TempAdapter);
		if (AdapterScore > BestAdapterScore)
		{
			BestAdapterScore = AdapterScore;
			BestAdapterIndex = Index;
		}
	}

	const HRESULT Result = Factory->EnumAdapterByGpuPreference(BestAdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter));
	RW_VERIFY_ID(Result);
	return NewRef<FGfxAdapter>(this, TempAdapter);
}

FNativeObject FGfxContext::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	switch (NativeObjectId)
	{
	case NativeObjectIds::DXGI_Factory: return Factory.Get();
	case NativeObjectIds::DXGI_Debug: return DXGIDebug.Get();
	default:
		break;
	}
	return IGfxObject::GetRawNativeObject(NativeObjectId);
}

uint8 FGfxContext::CalculateAdapterScore(const TComPtr<IDXGIAdapter4>& Adapter)
{
	PROFILE_FUNCTION();

	uint8 Score = 0;

	DXGI_ADAPTER_DESC3 AdapterDesc = {};
	HRESULT Result = Adapter->GetDesc3(&AdapterDesc);
	RW_VERIFY_ID(Result);

	RW_LOG(Info, "\t- {}", AdapterDesc.Description);

	if (!(AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_REMOTE) && !(AdapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
	{
		Score += 1;
		RW_LOG(Debug, "\t\t- IsDiscrete: +1");
	}

	TComPtr<ID3D12Device> TempDevice;
	Result = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
	RW_VERIFY_ID(Result);

	CD3DX12FeatureSupport FeatureSupport;
	FeatureSupport.Init(TempDevice.Get());
	if (FeatureSupport.RaytracingTier() != D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		uint8 RayScore = FeatureSupport.RaytracingTier() > D3D12_RAYTRACING_TIER_1_0 ? 2 : 1;
		Score += RayScore;
		RW_LOG(Debug, "\t\t- Raytracing: +{}", RayScore);
	}
	if (FeatureSupport.MeshShaderTier() != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
	{
		Score += 1;
		RW_LOG(Debug, "\t\t- MeshShader: +1");
	}
	if (FeatureSupport.VariableShadingRateTier() != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED)
	{
		uint8 ShadingScore = FeatureSupport.VariableShadingRateTier() > D3D12_VARIABLE_SHADING_RATE_TIER_1 ? 2 : 1;
		Score += ShadingScore;
		RW_LOG(Debug, "\t\t- VariableShading: +{}", ShadingScore);
	}
	if (FeatureSupport.WorkGraphsTier() != D3D12_WORK_GRAPHS_TIER_NOT_SUPPORTED)
	{
		Score += 1;
		RW_LOG(Debug, "\t\t- WorkGraphs: +1");
	}
	uint8 ResourceScore = static_cast<uint8>(FeatureSupport.ResourceBindingTier());
	Score += ResourceScore;
	RW_LOG(Debug, "\t\t- ResourceBinding: +{}", ResourceScore);
	RW_LOG(Info, "\t\t- Score: {}", Score);
	return Score;
}
