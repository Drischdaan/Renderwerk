#pragma once

#include "Renderwerk/Core/Containers/Queue.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"
#include "Renderwerk/Platform/Threading/CriticalSection.hpp"

class FGfxDescriptorHeap;

struct ENGINE_API FGfxDescriptorHandle
{
public:
	FGfxDescriptorHandle() = default;
	FGfxDescriptorHandle(FGfxDescriptorHeap* InOwnerHeap, uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle);
	FGfxDescriptorHandle(FGfxDescriptorHeap* InOwnerHeap, uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle,
	                     const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle);
	~FGfxDescriptorHandle() = default;

	DEFAULT_COPY_MOVEABLE(FGfxDescriptorHandle)

public:
	[[nodiscard]] bool8 IsValid() const;
	[[nodiscard]] bool8 IsGPUVisible() const;

	[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPUHandle; }
	[[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPUHandle; }

private:
	FGfxDescriptorHeap* OwnerHeap = nullptr;
	uint32 Index = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle = {};

	friend class FGfxDescriptorHeap;
};

struct ENGINE_API FGfxDescriptorHeapDesc
{
	D3D12_DESCRIPTOR_HEAP_TYPE Type;
	uint32 DescriptorCount;
};

class ENGINE_API FGfxDescriptorHeap : public IGfxDeviceChild
{
public:
	explicit FGfxDescriptorHeap(FGfxDevice* InGfxDevice, const FGfxDescriptorHeapDesc& InHeapDesc);
	FGfxDescriptorHeap(FGfxDevice* InGfxDevice, const FGfxDescriptorHeapDesc& InHeapDesc, const FStringView& InDebugName);
	~FGfxDescriptorHeap() override;

	NON_COPY_MOVEABLE(FGfxDescriptorHeap)

public:
	[[nodiscard]] FGfxDescriptorHandle Allocate();
	void Free(FGfxDescriptorHandle& DescriptorHandle);

public:
	[[nodiscard]] uint32 GetMaxHandleCount() const { return HeapDesc.DescriptorCount; }
	[[nodiscard]] uint32 GetAllocationCount() const { return AllocationCount; }

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	FGfxDescriptorHeapDesc HeapDesc;

	bool8 bIsGPUVisible;
	TComPtr<ID3D12DescriptorHeap> DescriptorHeap;

	uint32 DescriptorSize;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUStartHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUStartHandle;

	FCriticalSection IndicesSection;
	uint32 AllocationCount = 0;
	TQueue<uint32> FreeIndicesQueue;
};
