#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Span.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class FWindow;

struct ENGINE_API FGfxDeviceDesc
{
	uint32 MaxRenderTargets = 25;
	uint32 MaxShaderResources = 100;
};

class ENGINE_API FGfxDevice : public IGfxAdapterChild
{
public:
	explicit FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc);
	FGfxDevice(FGfxAdapter* InGfxAdapter, const FGfxDeviceDesc& InDeviceDesc, const FStringView& InDebugName);
	~FGfxDevice() override;

	NON_COPY_MOVEABLE(FGfxDevice)

public:
	void FlushGraphicsQueue() const;

	void SubmitGraphicsWork(const TRef<FGfxCommandList>& CommandList) const;

	[[nodiscard]] TRef<FGfxDescriptorHeap> CreateDescriptorHeap(const FGfxDescriptorHeapDesc& HeapDesc, const FStringView& DebugName = TEXT("UnnamedDescriptorHeap"));
	[[nodiscard]] TRef<FGfxSwapchain> CreateSwapchain(const FGfxSwapchainDesc& SwapchainDesc, const FStringView& DebugName = TEXT("UnnamedSwapchain"));
	[[nodiscard]] TRef<FGfxCommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE Type, const FStringView& DebugName = TEXT("UnnamedCommandList"));
	[[nodiscard]] TRef<FGfxFence> CreateFence(const FStringView& DebugName = TEXT("UnnamedFence"));
	[[nodiscard]] TRef<FGfxSurface> CreateSurface(const TRef<FWindow>& Window, const FStringView& DebugName = TEXT("UnnamedSurface"));
	[[nodiscard]] TRef<FGfxRootSignature> CreateRootSignature(const FStringView& DebugName = TEXT("UnnamedRootSignature"));
	[[nodiscard]] TRef<FGfxRootSignature> CreateRootSignature(const TVector<EGfxRootType>& RootTypes, size64 PushConstantSize,
	                                                          const FStringView& DebugName = TEXT("UnnamedRootSignature"));

public:
	template <typename T> requires std::is_base_of_v<IGfxRenderPass, T> && std::is_constructible_v<T, FGfxDevice*>
	[[nodiscard]] TRef<T> CreateRenderPass()
	{
		return NewRef<T>(this);
	}

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] TComPtr<ID3D12CommandQueue> GetCopyQueue() const { return CopyQueue; }

	[[nodiscard]] TRef<FGfxDescriptorHeap> GetRTVDescriptorHeap() const { return RTVDescriptorHeap; }
	[[nodiscard]] TRef<FGfxDescriptorHeap> GetSRVDescriptorHeap() const { return SRVDescriptorHeap; }

	[[nodiscard]] TRef<FGfxResourceManager> GetResourceManager() const { return ResourceManager; }
	[[nodiscard]] TRef<FGfxShaderCompiler> GetShaderCompiler() const { return ShaderCompiler; }

private:
	TComPtr<ID3D12CommandQueue> CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE Type) const;

private:
	FGfxDeviceDesc DeviceDesc;

	TComPtr<ID3D12Device14> Device;

	TComPtr<ID3D12CommandQueue> GraphicsQueue;
	TComPtr<ID3D12CommandQueue> ComputeQueue;
	TComPtr<ID3D12CommandQueue> CopyQueue;

	TRef<FGfxFence> GraphicsWorkFence;

	TRef<FGfxDescriptorHeap> RTVDescriptorHeap;
	TRef<FGfxDescriptorHeap> SRVDescriptorHeap;

	TRef<FGfxResourceManager> ResourceManager;
	TRef<FGfxShaderCompiler> ShaderCompiler;
};
