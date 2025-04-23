#include "pch.hpp"

#include "Renderwerk/Renderer/Device.hpp"

#include "Renderwerk/Renderer/Adapter.hpp"

FDevice::FDevice(FAdapter* InAdapter, const FDeviceDesc& InDeviceDesc)
	: DeviceDesc(InDeviceDesc), Adapter(InAdapter)
{
	const HRESULT Result = D3D12CreateDevice(Adapter->GetHandle().Get(), Adapter->GetFeatureLevel(), IID_PPV_ARGS(&Device));
	RW_VERIFY_ID(Result);

	GraphicsCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ComputeCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	CopyCommandQueue = CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
}

FDevice::~FDevice()
{
	CopyCommandQueue.Reset();
	ComputeCommandQueue.Reset();
	GraphicsCommandQueue.Reset();
	Device.Reset();
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
	return CommandQueue;
}
