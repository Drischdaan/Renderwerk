#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

struct ENGINE_API FGfxDeviceDesc
{
	uint32 MaxRenderTargets = 25;
};

class ENGINE_API FGfxDevice : public IGfxAdapterChild
{
public:
	explicit FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc);
	FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc, const FStringView& InDebugName);
	~FGfxDevice() override;

	NON_COPY_MOVEABLE(FGfxDevice)

public:
	[[nodiscard]] TRef<FGfxDescriptorHeap> CreateDescriptorHeap(const FGfxDescriptorHeapDesc& HeapDesc, const FStringView& DebugName = TEXT("UnnamedDescriptorHeap"));
	[[nodiscard]] TRef<FGfxSwapchain> CreateSwapchain(const FGfxSwapchainDesc& SwapchainDesc, const FStringView& DebugName = TEXT("UnnamedSwapchain"));

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetCopyQueue() const { return CopyQueue; }

	[[nodiscard]] TRef<FGfxDescriptorHeap> GetRTVDescriptorHeap() const { return RTVDescriptorHeap; }
	[[nodiscard]] TRef<FGfxResourceManager> GetResourceManager() const { return ResourceManager; }

private:
	TComPtr<ID3D12CommandQueue> CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE Type) const;

private:
	FGfxDeviceDesc DeviceDesc;

	TComPtr<ID3D12Device14> Device;

	TComPtr<ID3D12CommandQueue> GraphicsQueue;
	TComPtr<ID3D12CommandQueue> ComputeQueue;
	TComPtr<ID3D12CommandQueue> CopyQueue;

	TRef<FGfxDescriptorHeap> RTVDescriptorHeap;

	TRef<FGfxResourceManager> ResourceManager;
};
