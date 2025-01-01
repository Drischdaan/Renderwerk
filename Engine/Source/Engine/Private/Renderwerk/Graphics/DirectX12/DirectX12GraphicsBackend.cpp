#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsBackend.h"

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
