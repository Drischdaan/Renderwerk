#include "pch.hpp"

#include "Renderwerk/Renderer/Device.hpp"

#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Renderer/Adapter.hpp"
#include "Renderwerk/Renderer/ResourceUploader.hpp"

void InfoQueueCallback([[maybe_unused]] D3D12_MESSAGE_CATEGORY Category,
                       const D3D12_MESSAGE_SEVERITY Severity,
                       [[maybe_unused]] const D3D12_MESSAGE_ID Id,
                       LPCSTR Description,
                       [[maybe_unused]] void* Context)
{
	if (Id == D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE)
	{
		return;
	}
	switch (Severity)
	{
	case D3D12_MESSAGE_SEVERITY_CORRUPTION:
		FLogger::Log(ELogVerbosity::Critical, FStringUtilities::Format("{}", Description), FLoggerLocation::current());
		break;
	case D3D12_MESSAGE_SEVERITY_ERROR:
		FLogger::Log(ELogVerbosity::Error, FStringUtilities::Format("{}", Description), FLoggerLocation::current());
		break;
	case D3D12_MESSAGE_SEVERITY_WARNING:
		FLogger::Log(ELogVerbosity::Warning, FStringUtilities::Format("{}", Description), FLoggerLocation::current());
		break;
	case D3D12_MESSAGE_SEVERITY_INFO:
		FLogger::Log(ELogVerbosity::Info, FStringUtilities::Format("{}", Description), FLoggerLocation::current());
		break;
	case D3D12_MESSAGE_SEVERITY_MESSAGE:
		FLogger::Log(ELogVerbosity::Debug, FStringUtilities::Format("{}", Description), FLoggerLocation::current());
		break;
	}
}

FDevice::FDevice(FAdapter* InAdapter, const FDeviceDesc& InDeviceDesc)
	: DeviceDesc(InDeviceDesc), Adapter(InAdapter)
{
	HRESULT Result = D3D12CreateDevice(Adapter->GetHandle().Get(), Adapter->GetFeatureLevel(), IID_PPV_ARGS(&Device));
	RW_VERIFY_ID(Result);
	Device->SetName(TEXT("MainDevice"));

	if (DeviceDesc.bEnableInfoQueue)
	{
		Result = Device->QueryInterface(IID_PPV_ARGS(&InfoQueue));
		RW_VERIFY_ID(Result);

		Result = InfoQueue->RegisterMessageCallback(&InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie);
		RW_VERIFY_ID(Result);
	}

	GraphicsCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ComputeCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	CopyCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

	FDescriptorHeapDesc RTVHeapDesc;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVHeapDesc.DescriptorCount = DeviceDesc.MaxRenderTargets;
	RTVHeapDesc.DebugName = TEXT("RenderTargetHeap");
	RTVDescriptorHeap = NewObjectHandle<FDescriptorHeap>(this, RTVHeapDesc);

	FDescriptorHeapDesc SamplerHeapDesc;
	SamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	SamplerHeapDesc.DescriptorCount = DeviceDesc.MaxSamplers;
	SamplerHeapDesc.DebugName = TEXT("SamplerHeap");
	SamplerDescriptorHeap = NewObjectHandle<FDescriptorHeap>(this, SamplerHeapDesc);

	FDescriptorHeapDesc DSVHeapDesc;
	DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVHeapDesc.DescriptorCount = DeviceDesc.MaxDepthStencils;
	DSVHeapDesc.DebugName = TEXT("DepthStencilHeap");
	DSVDescriptorHeap = NewObjectHandle<FDescriptorHeap>(this, DSVHeapDesc);

	FDescriptorHeapDesc RSVHeapDesc;
	RSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	RSVHeapDesc.DescriptorCount = DeviceDesc.MaxShaderResources;
	RSVHeapDesc.DebugName = TEXT("ShaderResourceHeap");
	RSVDescriptorHeap = NewObjectHandle<FDescriptorHeap>(this, RSVHeapDesc);

	WorkFence = CreateFence();

	ResourceUploader = NewObjectHandle<FResourceUploader>(this);
}

FDevice::~FDevice()
{
	ResourceUploader.Reset();
	WorkFence.Reset();
	RSVDescriptorHeap.Reset();
	DSVDescriptorHeap.Reset();
	SamplerDescriptorHeap.Reset();
	RTVDescriptorHeap.Reset();
	CopyCommandQueue.Reset();
	ComputeCommandQueue.Reset();
	GraphicsCommandQueue.Reset();
	if (DeviceDesc.bEnableInfoQueue && InfoQueue)
	{
		InfoQueue->UnregisterMessageCallback(InfoQueueCookie);
	}
	InfoQueue.Reset();
	Device.Reset();
}

void FDevice::FlushWork() const
{
	const uint64 SignalValue = WorkFence->GetNextSignalValue();
	GraphicsCommandQueue->Signal(WorkFence->GetHandle(), WorkFence->GetNextSignalValue());
	WorkFence->SetSignaledValue(SignalValue);
	RW_VERIFY(WorkFence->Wait());
}

void FDevice::SubmitGraphicsWork(const TObjectHandle<FCommandList>& CommandList) const
{
	PROFILE_FUNCTION();
	ID3D12CommandList* CommandLists[] = {CommandList->GetHandle()};
	GraphicsCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
}

void FDevice::SignalGraphicsQueue(const TObjectHandle<FFence>& Fence) const
{
	PROFILE_FUNCTION();
	const uint64 SignalValue = Fence->GetNextSignalValue();
	const HRESULT Result = GraphicsCommandQueue->Signal(Fence->GetHandle(), SignalValue);
	RW_VERIFY_ID(Result);
	Fence->SetSignaledValue(SignalValue);
}

TObjectHandle<FFence> FDevice::CreateFence()
{
	return NewObjectHandle<FFence>(this);
}

TObjectHandle<FTexture> FDevice::CreateTexture(const FTextureDesc& TextureDesc)
{
	return NewObjectHandle<FTexture>(this, TextureDesc);
}

void FDevice::UploadBuffer(const TObjectHandle<FBuffer>& Buffer, const void* Data, const size64 DataSize) const
{
	ResourceUploader->QueueBufferUpload(Buffer, Data, DataSize);
}

void FDevice::UploadTexture(const TObjectHandle<FTexture>& Texture, const TVector<uint8>& Data) const
{
	ResourceUploader->QueueTextureUpload(Texture, Data);
}

TObjectHandle<ID3D12CommandQueue> FDevice::CreateInternalCommandQueue(const D3D12_COMMAND_LIST_TYPE Type) const
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	CommandQueueDesc.NodeMask = 0;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	CommandQueueDesc.Type = Type;

	TObjectHandle<ID3D12CommandQueue> CommandQueue;
	const HRESULT Result = Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue));
	RW_VERIFY_ID(Result);
	CommandQueue->SetName(TEXT("CustomCommandQueue"));
	return CommandQueue;
}
