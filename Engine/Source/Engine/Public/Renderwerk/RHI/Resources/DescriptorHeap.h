#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FDescriptorHeapDesc
{
	EDescriptorHeapType Type;
	uint32 Capacity;
};

class RENDERWERK_API FDescriptorHeap : public IDeviceChild
{
public:
	FDescriptorHeap(FDevice* InDevice, const FDescriptorHeapDesc& InDescription);
	~FDescriptorHeap() override;

	DELETE_COPY_AND_MOVE(FDescriptorHeap);

public:
	NODISCARD FDescriptorHandle AllocateDescriptor();
	void FreeDescriptor(const FDescriptorHandle& Handle);

	void CheckForLeaks();

public:
	NODISCARD TComPtr<ID3D12DescriptorHeap> GetHandle() const { return DescriptorHeap; }

	NODISCARD EDescriptorHeapType GetType() const { return Description.Type; }
	NODISCARD uint32 GetCapacity() const { return Description.Capacity; }

	NODISCARD bool8 IsShaderVisible() const { return bIsShaderVisible; }

private:
	FDescriptorHeapDesc Description;

	bool8 bIsShaderVisible;
	TComPtr<ID3D12DescriptorHeap> DescriptorHeap;
	uint32 DescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUStartHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUStartHandle;

	FMutex Mutex;
	TVector<FDescriptorHandle> DescriptorHandles;
	TDeque<uint32> FreeHandleIndices;
};
