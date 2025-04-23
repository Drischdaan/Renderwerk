#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FAdapter;

struct ENGINE_API FDeviceDesc
{
	uint32 MaxRenderTargets = 100;
};

class ENGINE_API FDevice : public TGraphicsObject<FDevice>
{
public:
	FDevice(FAdapter* InAdapter, const FDeviceDesc& InDeviceDesc);
	~FDevice() override;

	NON_COPY_MOVEABLE(FDevice)

public:
	[[nodiscard]] TObjectHandle<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] FAdapter* GetAdapter() const { return Adapter; }

	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetGraphicsCommandQueue() const { return GraphicsCommandQueue; }
	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetComputeCommandQueue() const { return ComputeCommandQueue; }
	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetCopyCommandQueue() const { return CopyCommandQueue; }

private:
	TObjectHandle<ID3D12CommandQueue> CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE Type) const;

private:
	FDeviceDesc DeviceDesc;

	FAdapter* Adapter;

	TObjectHandle<ID3D12Device14> Device;

	TObjectHandle<ID3D12CommandQueue> GraphicsCommandQueue;
	TObjectHandle<ID3D12CommandQueue> ComputeCommandQueue;
	TObjectHandle<ID3D12CommandQueue> CopyCommandQueue;
};
