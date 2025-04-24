#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/Queue.hpp"
#include "Renderwerk/Platform/Threading/CriticalSection.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDescriptorHeap;
class FDevice;

struct ENGINE_API FDescriptorHandle
{
public:
	FDescriptorHandle() = default;
	FDescriptorHandle(FDescriptorHeap* InOwnerHeap, uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle);
	FDescriptorHandle(FDescriptorHeap* InOwnerHeap, uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle);
	~FDescriptorHandle() = default;

	DEFAULT_COPY_MOVEABLE(FDescriptorHandle)

public:
	[[nodiscard]] bool8 IsValid() const;
	[[nodiscard]] bool8 IsGPUVisible() const;

	[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPUHandle; }
	[[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPUHandle; }

private:
	FDescriptorHeap* OwnerHeap = nullptr;
	uint32 Index = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle = {};

	friend class FDescriptorHeap;
};

struct ENGINE_API FDescriptorHeapDesc
{
	D3D12_DESCRIPTOR_HEAP_TYPE Type;
	uint32 DescriptorCount;
	FStringView DebugName = TEXT("DescriptorHeap");
};

class ENGINE_API FDescriptorHeap : public TGraphicsObject<FDescriptorHeap>
{
public:
	FDescriptorHeap(FDevice* InDevice, const FDescriptorHeapDesc& InHeapDesc);
	~FDescriptorHeap() override;

	NON_COPY_MOVEABLE(FDescriptorHeap)

public:
	[[nodiscard]] FDescriptorHandle Allocate();
	void Free(FDescriptorHandle& DescriptorHandle);

public:
	[[nodiscard]] size64 GetAllocationCount() const { return AllocationCount; }

private:
	FDevice* Device;
	FDescriptorHeapDesc HeapDesc;

	bool8 bIsGPUVisible;
	TObjectHandle<ID3D12DescriptorHeap> DescriptorHeap;

	uint32 DescriptorSize;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUStartHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUStartHandle;

	FCriticalSection IndicesSection;
	uint32 AllocationCount = 0;
	TQueue<uint32> FreeIndicesQueue;
};
