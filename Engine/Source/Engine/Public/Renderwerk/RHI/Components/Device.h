#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FDevice : public IAdapterChild
{
public:
	FDevice(FAdapter* Adapter);
	~FDevice() override;

	DELETE_COPY_AND_MOVE(FDevice);

public:
	void WaitForQueueIdle(const TSharedPtr<FCommandQueue>& CommandQueue);
	void WaitForGraphicsQueueIdle();

	NODISCARD TSharedPtr<FCommandQueue> CreateCommandQueue(ECommandListType Type);
	NODISCARD TComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ECommandListType Type) const;
	NODISCARD TSharedPtr<FCommandList> CreateCommandList(ECommandListType Type, ID3D12CommandAllocator* Allocator);

	NODISCARD TSharedPtr<FFence> CreateFence(uint64 InitialValue = 0);

	NODISCARD TSharedPtr<FDescriptorHeap> CreateDescriptorHeap(const FDescriptorHeapDesc& Description);

	NODISCARD TSharedPtr<FSwapchain> CreateSwapchain(const FSwapchainDesc& Description);

	/**
	 * @brief Function that forwards the capabilities of the adapter.
	 */
	NODISCARD const FAdapterCapabilities& GetCapabilities() const;

public:
	NODISCARD TComPtr<ID3D12Device14> GetHandle() const { return Device; }

	NODISCARD TSharedPtr<FCommandQueue> GetGraphicsQueue() const { return GraphicsQueue; }
	NODISCARD TSharedPtr<FCommandQueue> GetComputeQueue() const { return ComputeQueue; }
	NODISCARD TSharedPtr<FCommandQueue> GetCopyQueue() const { return CopyQueue; }

	NODISCARD TSharedPtr<FDescriptorHeap> GetRenderTargetViewHeap() const { return RenderTargetViewHeap; }

private:
	TComPtr<ID3D12Device14> Device;

#if RW_ENABLE_GPU_DEBUGGING
	TComPtr<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie = 0;
#endif

	TSharedPtr<FCommandQueue> GraphicsQueue;
	TSharedPtr<FCommandQueue> ComputeQueue;
	TSharedPtr<FCommandQueue> CopyQueue;

	TSharedPtr<FDescriptorHeap> RenderTargetViewHeap;
};
