#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FDescriptorHandle
{
public:
	FDescriptorHandle();
	FDescriptorHandle(const EDescriptorHeapType& InType, uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle,
	                  const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle);
	~FDescriptorHandle() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDescriptorHandle);

public:
	NODISCARD bool8 IsValid() const;

public:
	NODISCARD EDescriptorHeapType GetType() const { return Type; }
	NODISCARD uint32 GetIndex() const { return Index; }

	NODISCARD CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPUHandle; }
	NODISCARD CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPUHandle; }

	operator CD3DX12_CPU_DESCRIPTOR_HANDLE() const { return CPUHandle; }
	operator CD3DX12_GPU_DESCRIPTOR_HANDLE() const { return GPUHandle; }

private:
	EDescriptorHeapType Type;
	uint32 Index;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;
};
