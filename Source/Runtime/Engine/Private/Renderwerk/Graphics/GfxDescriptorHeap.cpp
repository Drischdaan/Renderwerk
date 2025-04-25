#include "pch.hpp"

#include "Renderwerk/Graphics/GfxDescriptorHeap.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Platform/Threading/ScopedLock.hpp"

FGfxDescriptorHandle::FGfxDescriptorHandle(FGfxDescriptorHeap* InOwnerHeap, const uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle)
	: OwnerHeap(InOwnerHeap), Index(InIndex), CPUHandle(InCPUHandle)
{
}

FGfxDescriptorHandle::FGfxDescriptorHandle(FGfxDescriptorHeap* InOwnerHeap, const uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle,
                                           const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle)
	: OwnerHeap(InOwnerHeap), Index(InIndex), CPUHandle(InCPUHandle), GPUHandle(InGPUHandle)
{
}

bool8 FGfxDescriptorHandle::IsValid() const
{
	return OwnerHeap != nullptr && CPUHandle.ptr != 0;
}

bool8 FGfxDescriptorHandle::IsGPUVisible() const
{
	return GPUHandle.ptr != 0;
}

FGfxDescriptorHeap::FGfxDescriptorHeap(FGfxDevice* InGfxDevice, const FGfxDescriptorHeapDesc& InHeapDesc)
	: FGfxDescriptorHeap(InGfxDevice, InHeapDesc, TEXT("UnnamedDescriptorHeap"))
{
}

FGfxDescriptorHeap::FGfxDescriptorHeap(FGfxDevice* InGfxDevice, const FGfxDescriptorHeapDesc& InHeapDesc, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), HeapDesc(InHeapDesc)
{
	bIsGPUVisible = HeapDesc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || HeapDesc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Flags = bIsGPUVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.Type = HeapDesc.Type;
	DescriptorHeapDesc.NumDescriptors = HeapDesc.DescriptorCount;

	ID3D12Device* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device>(NativeObjectIds::D3D12_Device);

	const HRESULT Result = NativeDevice->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
	RW_VERIFY_ID(Result);

	DescriptorSize = NativeDevice->GetDescriptorHandleIncrementSize(HeapDesc.Type);
	CPUStartHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (bIsGPUVisible)
	{
		GPUStartHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}

	for (uint32 Index = 0; Index < HeapDesc.DescriptorCount; ++Index)
	{
		FreeIndicesQueue.push(Index);
	}

	RW_LOG(Debug, "Created descriptor heap ('{}') of type {} with {} handles available", GetDebugName(), D3DUtility::ToString(HeapDesc.Type), HeapDesc.DescriptorCount);
}

FGfxDescriptorHeap::~FGfxDescriptorHeap()
{
	DescriptorHeap.Reset();
}

FGfxDescriptorHandle FGfxDescriptorHeap::Allocate()
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

void FGfxDescriptorHeap::Free(FGfxDescriptorHandle& DescriptorHandle)
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

FNativeObject FGfxDescriptorHeap::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_DescriptorHeap)
	{
		return DescriptorHeap.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}
