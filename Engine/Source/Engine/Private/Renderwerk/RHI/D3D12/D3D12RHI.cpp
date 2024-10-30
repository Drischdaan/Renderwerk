#include "pch.h"

#include "Renderwerk/RHI/D3D12/D3D12RHI.h"

#include "Renderwerk/Platform/Platform.h"

#include "Renderwerk/RHI/D3D12/D3D12Adapter.h"

DEFINE_LOG_CATEGORY(LogD3D12RHI);

FD3D12RHI::FD3D12RHI(const FRHIDesc& InDescription)
	: IRHI(InDescription)
{
	if (Description.bEnableDebugLayer)
	{
		DEBUG_D3D_CHECKM(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug)), "Failed to get DXGI debug interface")
		DXGIDebug->EnableLeakTrackingForThread();
		RW_LOG(LogD3D12RHI, Info, "Enabled leak tracking for thread {}", GetPlatform()->GetCurrentThreadId());

		DEBUG_D3D_CHECKM(D3D12GetDebugInterface(IID_PPV_ARGS(&D3DDebug)), "Failed to get D3D12 debug interface")
		D3DDebug->EnableDebugLayer();
		D3DDebug->SetEnableAutoName(true);

		RW_LOG(LogD3D12RHI, Warn, "Enabled D3D12 debug layer. This may impact performance");

		if (Description.bEnableValidationLayer)
		{
			D3DDebug->SetEnableGPUBasedValidation(true);
			D3DDebug->SetEnableSynchronizedCommandQueueValidation(true);

			RW_LOG(LogD3D12RHI, Warn, "Enabled D3D12 validation layer. This will significantly impact performance");
		}
	}

	uint32 FactoryCreationFlags = Description.bEnableDebugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
	D3D_CHECKM(CreateDXGIFactory2(FactoryCreationFlags, IID_PPV_ARGS(&Factory)), "Failed to create DXGI factory")

	RW_LOG(LogD3D12RHI, Info, "D3D12 RHI initialized");
}

FD3D12RHI::~FD3D12RHI()
{
	Factory.Reset();
	D3DDebug.Reset();
	if (DXGIDebug && Description.bEnableDebugLayer)
	{
		OutputDebugString(TEXT("DXGI Leak Report:\n"));
		DXGI_DEBUG_RLO_FLAGS Flags = static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		DEBUG_D3D_CHECK(DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, Flags))
	}
	DXGIDebug.Reset();
}

TVector<TSharedPtr<IAdapter>> FD3D12RHI::QueryAdapters()
{
	TVector<TSharedPtr<IAdapter>> Adapters;
	TComPtr<IDXGIAdapter4> TempAdapter;
	for (uint32 Index = 0; Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) != DXGI_ERROR_NOT_FOUND; ++Index)
		Adapters.push_back(MakeShared<FD3D12Adapter>(Index, Factory, TempAdapter));
	return Adapters;
}
