#include "pch.h"

#include "Renderwerk/RHI/Resources/DescriptorHandle.h"

FDescriptorHandle::FDescriptorHandle()
	: Type(EDescriptorHeapType::None), Index(0), CPUHandle(), GPUHandle()
{
}

FDescriptorHandle::FDescriptorHandle(const EDescriptorHeapType& InType, const uint32 InIndex, const CD3DX12_CPU_DESCRIPTOR_HANDLE& InCPUHandle,
                                     const CD3DX12_GPU_DESCRIPTOR_HANDLE& InGPUHandle)
	: Type(InType), Index(InIndex), CPUHandle(InCPUHandle), GPUHandle(InGPUHandle)
{
}

bool8 FDescriptorHandle::IsValid() const
{
	return Type != EDescriptorHeapType::None;
}
