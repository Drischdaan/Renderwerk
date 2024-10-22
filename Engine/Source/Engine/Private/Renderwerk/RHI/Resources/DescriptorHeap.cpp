#include "pch.h"

#include "Renderwerk/RHI/Resources/DescriptorHeap.h"
#include "Renderwerk/RHI/Resources/DescriptorHandle.h"

FDescriptorHeap::FDescriptorHeap(FDevice* InDevice, const FDescriptorHeapDesc& InDescription)
	: IDeviceChild(TEXT("DescriptorHeap"), InDevice), Description(InDescription)
{
	bIsShaderVisible = Description.Type == EDescriptorHeapType::ShaderAssetView || Description.Type == EDescriptorHeapType::Sampler;

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Flags = bIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(Description.Type);
	DescriptorHeapDesc.NumDescriptors = Description.Capacity;
	FD3DResult CreateResult = GetDeviceHandle()->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
	D3D_CHECKM(CreateResult, "Failed to create descriptor heap");

	DescriptorSize = GetDeviceHandle()->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
	CPUStartHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (bIsShaderVisible)
		GPUStartHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	DescriptorHandles.resize(Description.Capacity);
	for (uint32 Index = 0; Index < Description.Capacity; ++Index)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(CPUStartHandle, Index, DescriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;
		if (bIsShaderVisible)
			GPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUStartHandle, Index, DescriptorSize);
		DescriptorHandles[Index] = {
			Description.Type,
			Index,
			CPUHandle,
			GPUHandle,
		};
		FreeHandleIndices.push_back(Index);
	}
}

FDescriptorHeap::~FDescriptorHeap()
{
	DescriptorHeap.Reset();
}

FDescriptorHandle FDescriptorHeap::AllocateDescriptor()
{
	RW_PROFILING_MARK_FUNCTION();

	FScopedLock Lock(Mutex);
	DEBUG_D3D_CHECKM(!FreeHandleIndices.empty(), "Descriptor heap is full");
	FDescriptorHandle Handle = DescriptorHandles.at(FreeHandleIndices.front());
	FreeHandleIndices.pop_front();
	return Handle;
}

void FDescriptorHeap::FreeDescriptor(const FDescriptorHandle& Handle)
{
	RW_PROFILING_MARK_FUNCTION();

	FScopedLock Lock(Mutex);
	FreeHandleIndices.push_back(Handle.GetIndex());
}

void FDescriptorHeap::CheckForLeaks()
{
	FScopedLock Lock(Mutex);
	if (FreeHandleIndices.size() != DescriptorHandles.size())
	{
		Lock.Unlock();
		RW_LOG(LogRHI, Warn, "Descriptor heap has leaked descriptors");
	}
}
