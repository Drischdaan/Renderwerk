#include "pch.h"

#include "Renderwerk/RHI/Components/Device.h"
#include "Renderwerk/RHI/Commands/CommandList.h"
#include "Renderwerk/RHI/Commands/CommandQueue.h"
#include "Renderwerk/RHI/Components/Adapter.h"
#include "Renderwerk/RHI/Components/Swapchain.h"
#include "Renderwerk/RHI/Resources/DescriptorHeap.h"
#include "Renderwerk/RHI/Synchronization/Fence.h"

DECLARE_LOG_CATEGORY(LogD3D12, Trace);

DEFINE_LOG_CATEGORY(LogD3D12);

#if RW_ENABLE_GPU_DEBUGGING
void InfoQueueCallback(D3D12_MESSAGE_CATEGORY Category, const D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID Id, const LPCSTR Description, void* Context)
{
	FString Message = FStringUtils::ConvertToWideString(Description);
	if (Severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
		RW_LOG(LogD3D12, Debug, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_INFO)
		RW_LOG(LogD3D12, Info, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_WARNING)
		RW_LOG(LogD3D12, Warn, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_ERROR)
		RW_LOG(LogD3D12, Error, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_CORRUPTION)
		RW_LOG(LogD3D12, Fatal, "{}", Message);
}
#endif

FDevice::FDevice(FAdapter* Adapter)
	: IAdapterChild(TEXT("Device"), Adapter)
{
	IDXGIAdapter4* AdapterHandle = Adapter->GetHandle().Get();
	D3D_FEATURE_LEVEL FeatureLevel = static_cast<D3D_FEATURE_LEVEL>(Adapter->GetCapabilities().FeatureLevel);
	FD3DResult CreateResult = D3D12CreateDevice(AdapterHandle, FeatureLevel, IID_PPV_ARGS(&Device));
	D3D_CHECKM(CreateResult, "Failed to create device");
	D3D12_SET_NAME(Device, GetObjectName().c_str());

#if RW_ENABLE_GPU_DEBUGGING
	DEBUG_D3D_CHECKM(Device.As(&InfoQueue), "Failed to get info queue");
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));

	HRESULT RegisterResult = InfoQueue->RegisterMessageCallback(InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie);
	DEBUG_D3D_CHECKM(RegisterResult, "Failed to register message callback");
	RW_LOG(LogRHI, Debug, "Registered info queue callback");
#endif

	GraphicsQueue = CreateCommandQueue(ECommandListType::Graphics);
	RHI_SET_NAME(GraphicsQueue, TEXT("GraphicsCommandQueue"));

	ComputeQueue = CreateCommandQueue(ECommandListType::Compute);
	RHI_SET_NAME(ComputeQueue, TEXT("ComputeCommandQueue"));

	CopyQueue = CreateCommandQueue(ECommandListType::Copy);
	RHI_SET_NAME(CopyQueue, TEXT("CopyCommandQueue"));

	FDescriptorHeapDesc DescriptorHeapDesc;
	DescriptorHeapDesc.Type = EDescriptorHeapType::RenderTargetView;
	DescriptorHeapDesc.Capacity = 10;
	RenderTargetViewHeap = CreateDescriptorHeap(DescriptorHeapDesc);
}

FDevice::~FDevice()
{
	if (RenderTargetViewHeap)
		RenderTargetViewHeap->CheckForLeaks();
	RenderTargetViewHeap.reset();
	CopyQueue.reset();
	ComputeQueue.reset();
	GraphicsQueue.reset();
#if RW_ENABLE_GPU_DEBUGGING
	if (InfoQueue)
	{
		DEBUG_D3D_CHECKM(InfoQueue->UnregisterMessageCallback(InfoQueueCookie), "Failed to unregister message callback");
	}
	InfoQueue.Reset();
#endif
	Device.Reset();
}

void FDevice::WaitForQueueIdle(const TSharedPtr<FCommandQueue>& CommandQueue)
{
	RW_PROFILING_MARK_FUNCTION();

	TSharedPtr<FFence> Fence = CreateFence();
	Fence->SignalGPU(CommandQueue);
	Fence->WaitOnCPU();
}

void FDevice::WaitForGraphicsQueueIdle()
{
	WaitForQueueIdle(GraphicsQueue);
}

TSharedPtr<FCommandQueue> FDevice::CreateCommandQueue(ECommandListType Type)
{
	return MakeShared<FCommandQueue>(this, Type);
}

TComPtr<ID3D12CommandAllocator> FDevice::CreateCommandAllocator(ECommandListType Type) const
{
	TComPtr<ID3D12CommandAllocator> Allocator;
	D3D_CHECKM(Device->CreateCommandAllocator(static_cast<D3D12_COMMAND_LIST_TYPE>(Type), IID_PPV_ARGS(&Allocator)), "Failed to create command allocator");
	return Allocator;
}

TSharedPtr<FCommandList> FDevice::CreateCommandList(ECommandListType Type, ID3D12CommandAllocator* Allocator)
{
	return MakeShared<FCommandList>(this, Type, Allocator);
}

TSharedPtr<FFence> FDevice::CreateFence(uint64 InitialValue)
{
	return MakeShared<FFence>(this, InitialValue);
}

TSharedPtr<FDescriptorHeap> FDevice::CreateDescriptorHeap(const FDescriptorHeapDesc& Description)
{
	return MakeShared<FDescriptorHeap>(this, Description);
}

TSharedPtr<FSwapchain> FDevice::CreateSwapchain(const FSwapchainDesc& Description)
{
	return MakeShared<FSwapchain>(this, Description);
}

const FAdapterCapabilities& FDevice::GetCapabilities() const
{
	return GetAdapter()->GetCapabilities();
}
