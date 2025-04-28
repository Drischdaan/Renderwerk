#include "pch.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"

#include "Renderwerk/Core/Containers/Array.hpp"
#include "Renderwerk/Graphics/GfxAdapter.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDescriptorHeap.hpp"
#include "Renderwerk/Graphics/GfxFence.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxRootSignature.hpp"
#include "Renderwerk/Graphics/GfxShaderCompiler.hpp"
#include "Renderwerk/Graphics/GfxSurface.hpp"
#include "Renderwerk/Graphics/GfxSwapchain.hpp"

FGfxDevice::FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc)
	: FGfxDevice(InGfxAdapter, InDeviceDesc, TEXT("UnnamedDevice"))
{
}

FGfxDevice::FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc, const FStringView& InDebugName)
	: IGfxAdapterChild(InGfxAdapter, InDebugName), DeviceDesc(InDeviceDesc)
{
	IDXGIAdapter4* Adapter = GfxAdapter->GetNativeObject<IDXGIAdapter4>(NativeObjectIds::DXGI_Adapter);
	const HRESULT Result = D3D12CreateDevice(Adapter, GfxAdapter->GetFeatureLevel(), IID_PPV_ARGS(&Device));
	RW_VERIFY_ID(Result);

	GraphicsQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ComputeQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	CopyQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

	GraphicsWorkFence = CreateFence();
	CopyWorkFence = CreateFence();

	FGfxDescriptorHeapDesc RTVHeapDesc;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVHeapDesc.DescriptorCount = DeviceDesc.MaxRenderTargets;
	RTVDescriptorHeap = CreateDescriptorHeap(RTVHeapDesc, TEXT("RTVDescriptorHeap"));

	FGfxDescriptorHeapDesc SRVHeapDesc;
	SRVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRVHeapDesc.DescriptorCount = DeviceDesc.MaxShaderResources;
	SRVDescriptorHeap = CreateDescriptorHeap(SRVHeapDesc, TEXT("SRVDescriptorHeap"));

	FGfxResourceManagerDesc ResourceManagerDesc = {};
	ResourceManager = NewRef<FGfxResourceManager>(this, ResourceManagerDesc);

	ShaderCompiler = NewRef<FGfxShaderCompiler>(this, GfxAdapter->GetShaderModel());

	RW_LOG(Debug, "Created device ('{}') for '{}' adapter with {}", GetDebugName(), GfxAdapter->GetName(), D3DUtility::ToString(GfxAdapter->GetFeatureLevel()));
}

FGfxDevice::~FGfxDevice()
{
	ShaderCompiler.reset();
	ResourceManager.reset();

	SRVDescriptorHeap.reset();
	RTVDescriptorHeap.reset();

	CopyWorkFence.reset();
	GraphicsWorkFence.reset();

	CopyQueue.Reset();
	ComputeQueue.Reset();
	GraphicsQueue.Reset();

	Device.Reset();
}

void FGfxDevice::FlushGraphicsQueue() const
{
	GraphicsWorkFence->SignalCommandQueue(GraphicsQueue);
	RW_VERIFY(GraphicsWorkFence->Wait());
}

void FGfxDevice::FlushCopyQueue() const
{
	CopyWorkFence->SignalCommandQueue(CopyQueue);
	RW_VERIFY(CopyWorkFence->Wait());
}

void FGfxDevice::SubmitGraphicsWork(const TRef<FGfxCommandList>& CommandList) const
{
	const TArray CommandLists = {
		CommandList->GetNativeObject<ID3D12CommandList>(NativeObjectIds::D3D12_CommandList),
	};
	// ReSharper disable once CppRedundantCastExpression
	GraphicsQueue->ExecuteCommandLists(static_cast<uint32>(CommandLists.size()), CommandLists.data());
}

void FGfxDevice::SubmitCopyWork(const TRef<FGfxCommandList>& CommandList) const
{
	const TArray CommandLists = {
		CommandList->GetNativeObject<ID3D12CommandList>(NativeObjectIds::D3D12_CommandList),
	};
	// ReSharper disable once CppRedundantCastExpression
	CopyQueue->ExecuteCommandLists(static_cast<uint32>(CommandLists.size()), CommandLists.data());
}

TRef<FGfxDescriptorHeap> FGfxDevice::CreateDescriptorHeap(const FGfxDescriptorHeapDesc& HeapDesc, const FStringView& DebugName)
{
	return NewRef<FGfxDescriptorHeap>(this, HeapDesc, DebugName);
}

TRef<FGfxSwapchain> FGfxDevice::CreateSwapchain(const FGfxSwapchainDesc& SwapchainDesc, const FStringView& DebugName)
{
	return NewRef<FGfxSwapchain>(this, SwapchainDesc, DebugName);
}

TRef<FGfxCommandList> FGfxDevice::CreateCommandList(D3D12_COMMAND_LIST_TYPE Type, const FStringView& DebugName)
{
	return NewRef<FGfxCommandList>(this, Type, DebugName);
}

TRef<FGfxFence> FGfxDevice::CreateFence(const FStringView& DebugName)
{
	return NewRef<FGfxFence>(this, DebugName);
}

TRef<FGfxSurface> FGfxDevice::CreateSurface(const TRef<FWindow>& Window, const FStringView& DebugName)
{
	return NewRef<FGfxSurface>(this, Window, DebugName);
}

TRef<FGfxRootSignature> FGfxDevice::CreateRootSignature(const FStringView& DebugName)
{
	return NewRef<FGfxRootSignature>(this, DebugName);
}

TRef<FGfxRootSignature> FGfxDevice::CreateRootSignature(const TVector<EGfxRootType>& RootTypes, size64 PushConstantSize, const FStringView& DebugName)
{
	return NewRef<FGfxRootSignature>(this, RootTypes, PushConstantSize, DebugName);
}

FNativeObject FGfxDevice::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	switch (NativeObjectId)
	{
	case NativeObjectIds::D3D12_Device: return Device.Get();
	case NativeObjectIds::D3D12_GraphicsCommandQueue: return GraphicsQueue.Get();
	case NativeObjectIds::D3D12_ComputeCommandQueue: return ComputeQueue.Get();
	case NativeObjectIds::D3D12_CopyCommandQueue: return CopyQueue.Get();
	default:
		break;
	}
	return IGfxAdapterChild::GetRawNativeObject(NativeObjectId);
}

TComPtr<ID3D12CommandQueue> FGfxDevice::CreateInternalCommandQueue(const D3D12_COMMAND_LIST_TYPE Type) const
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	CommandQueueDesc.NodeMask = 0;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	CommandQueueDesc.Type = Type;

	TComPtr<ID3D12CommandQueue> TempCommandQueue;
	const HRESULT Result = Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&TempCommandQueue));
	RW_VERIFY_ID(Result);
	return TempCommandQueue;
}
