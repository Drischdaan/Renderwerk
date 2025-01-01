#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsBackend.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsAdapter.h"
#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsWindowContext.h"

FDirectX12GraphicsBackend::FDirectX12GraphicsBackend()
	: IGraphicsBackend(EGraphicsBackendType::DirectX12)
{
}

FDirectX12GraphicsBackend::~FDirectX12GraphicsBackend() = default;

void FDirectX12GraphicsBackend::Initialize(const FGraphicsBackendDesc& InDescription)
{
	Description = InDescription;

	if (Description.bEnableDebugging || Description.bEnableGpuValidation)
	{
		HRESULT Result = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug));
		ASSERT(SUCCEEDED(Result), "Failed to create DXGI Debug interface");

		Result = D3D12GetDebugInterface(IID_PPV_ARGS(&D3DDebug));
		ASSERT(SUCCEEDED(Result), "Failed to create D3D12 Debug interface");

		if (Description.bEnableDebugging)
		{
			DXGIDebug->EnableLeakTrackingForThread();
			D3DDebug->EnableDebugLayer();
			D3DDebug->SetEnableAutoName(true);
		}

		if (Description.bEnableGpuValidation)
		{
			D3DDebug->SetEnableGPUBasedValidation(true);
			D3DDebug->SetEnableSynchronizedCommandQueueValidation(true);
		}
	}

	const uint32 DXGIFactoryFlags = Description.bEnableDebugging ? DXGI_CREATE_FACTORY_DEBUG : 0;
	const HRESULT Result = CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&Factory));
	ASSERT(SUCCEEDED(Result), "Failed to create DXGI Factory");

	RW_LOG(LogGraphics, Info, "DirectX12 backend initialized");
}

void FDirectX12GraphicsBackend::Destroy()
{
	Factory.Reset();
	if (Description.bEnableDebugging || Description.bEnableGpuValidation)
	{
		D3DDebug.Reset();
		if (Description.bEnableDebugging)
		{
			RW_LOG(LogGraphics, Debug, "Dumping live objects into debug console...");
			DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
		DXGIDebug.Reset();
	}
}

TVector<TSharedPtr<IGraphicsAdapter>> FDirectX12GraphicsBackend::GetAvailableAdapters()
{
	TVector<TSharedPtr<IGraphicsAdapter>> Adapters;
	ComPtr<IDXGIAdapter4> Adapter;
	for (int Index = 0; Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter)) != DXGI_ERROR_NOT_FOUND; ++Index)
	{
		TSharedPtr<FDirectX12GraphicsAdapter> DirectX12Adapter = MakeShared<FDirectX12GraphicsAdapter>(this, Adapter);
		Adapters.emplace_back(DirectX12Adapter);
	}
	return Adapters;
}

bool8 FDirectX12GraphicsBackend::IsAdapterSuitable(const TSharedPtr<IGraphicsAdapter>& GraphicsAdapter)
{
	const FDirectX12GraphicsAdapter& Adapter = GraphicsAdapter->As<FDirectX12GraphicsAdapter>();
	if (Adapter.GetProperties().Type != EGraphicsAdapterType::Discrete)
	{
		RW_LOG(LogGraphics, Trace, "Skipping adapter. '{}' is not discrete", Adapter.GetProperties().Name);
		return false;
	}
	if (Adapter.GetFeatureSupport().MaxSupportedFeatureLevel() < D3D_FEATURE_LEVEL_12_0)
	{
		RW_LOG(LogGraphics, Trace, "Skipping adapter. '{}' does not support feature level 12 or higher", Adapter.GetProperties().Name);
		return false;
	}
	if (Adapter.GetFeatureSupport().HighestShaderModel() < D3D_SHADER_MODEL_6_8)
	{
		RW_LOG(LogGraphics, Trace, "Skipping adapter. '{}' does not support shader model 6.8 or higher", Adapter.GetProperties().Name);
		return false;
	}
	if (Adapter.GetFeatureSupport().MeshShaderTier() == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
	{
		RW_LOG(LogGraphics, Trace, "Skipping adapter. '{}' does not support mesh shaders", Adapter.GetProperties().Name);
		return false;
	}
	return true;
}

TSharedPtr<IGraphicsWindowContext> FDirectX12GraphicsBackend::CreateWindowContext()
{
	return MakeShared<FDirectX12GraphicsWindowContext>(this);
}
