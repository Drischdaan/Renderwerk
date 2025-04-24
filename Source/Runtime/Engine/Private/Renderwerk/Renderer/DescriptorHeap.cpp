#include "pch.hpp"

#include "Renderwerk/Renderer/DescriptorHeap.hpp"

#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Renderer/Device.hpp"

FDescriptorHandle::FDescriptorHandle(FDescriptorHeap* InOwnerHeap, const uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle)
	: OwnerHeap(InOwnerHeap), Index(InIndex), CPUHandle(InCPUHandle)
{
}

FDescriptorHandle::FDescriptorHandle(FDescriptorHeap* InOwnerHeap, const uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle,
                                     const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle)
	: OwnerHeap(InOwnerHeap), Index(InIndex), CPUHandle(InCPUHandle), GPUHandle(InGPUHandle)
{
}

bool8 FDescriptorHandle::IsValid() const
{
	return OwnerHeap != nullptr && CPUHandle.ptr != 0;
}

bool8 FDescriptorHandle::IsGPUVisible() const
{
	return GPUHandle.ptr != 0;
}

FDescriptorHeap::FDescriptorHeap(FDevice* InDevice, const FDescriptorHeapDesc& InHeapDesc)
	: Device(InDevice), HeapDesc(InHeapDesc)
{
	bIsGPUVisible = HeapDesc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || HeapDesc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Flags = bIsGPUVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.Type = HeapDesc.Type;
	DescriptorHeapDesc.NumDescriptors = HeapDesc.DescriptorCount;

	const HRESULT Result = Device->GetHandle()->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
	RW_VERIFY_ID(Result);
	DescriptorHeap->SetName(HeapDesc.DebugName.data());

	DescriptorSize = Device->GetHandle()->GetDescriptorHandleIncrementSize(HeapDesc.Type);

	CPUStartHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (bIsGPUVisible)
	{
		GPUStartHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}

	for (uint32 Index = 0; Index < HeapDesc.DescriptorCount; ++Index)
	{
		FreeIndicesQueue.push(Index);
	}
}

FDescriptorHeap::~FDescriptorHeap()
{
	DescriptorHeap.Reset();
}

FDescriptorHandle FDescriptorHeap::Allocate()
{
	FScopedLock Lock(&IndicesSection);
	RW_VERIFY_MSG(!FreeIndicesQueue.empty() && AllocationCount < HeapDesc.DescriptorCount, "Descriptor heap out of free descriptors");

	const uint32 Index = FreeIndicesQueue.front();
	FreeIndicesQueue.pop();
	++AllocationCount;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;
	if (bIsGPUVisible)
	{
		GPUHandle.InitOffsetted(GPUStartHandle, static_cast<int32>(Index), DescriptorSize);
	}
	return {this, Index, CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUStartHandle, static_cast<int32>(Index), DescriptorSize), GPUHandle};
}

void FDescriptorHeap::Free(FDescriptorHandle& DescriptorHandle)
{
	FScopedLock Lock(&IndicesSection);
	RW_VERIFY(DescriptorHandle.OwnerHeap == this);
	RW_VERIFY_MSG(DescriptorHandle.Index < HeapDesc.DescriptorCount, "Attempting to free an out-of-range descriptor index");
	RW_VERIFY_MSG(DescriptorHandle.CPUHandle.ptr != 0, "Attempting to free an already freed descriptor handle");

	FreeIndicesQueue.push(DescriptorHandle.Index);
	--AllocationCount;

	DescriptorHandle.OwnerHeap = nullptr;
	DescriptorHandle.Index = 0;
	DescriptorHandle.CPUHandle.ptr = 0;
	DescriptorHandle.GPUHandle.ptr = 0;
}
