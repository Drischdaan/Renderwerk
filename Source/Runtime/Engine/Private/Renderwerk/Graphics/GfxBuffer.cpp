#include "pch.hpp"

#include "Renderwerk/Graphics/GfxBuffer.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice, const EGfxBufferType InType, const uint64 InSize)
	: FGfxBuffer(InGfxDevice, InType, InSize, TEXT("UnnamedBuffer"))
{
}

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice, const EGfxBufferType InType, const uint64 InSize, const FStringView& InDebugName)
	: IGfxResource(InGfxDevice, InDebugName), Type(InType)
{
	AllocationSize = InSize;

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDesc.Width = AllocationSize;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	if (Type == EGfxBufferType::Storage)
	{
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	CD3DX12_HEAP_PROPERTIES HeapProperties = {};
	switch (Type)
	{
	case EGfxBufferType::Constant:
	case EGfxBufferType::Copy:
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case EGfxBufferType::Readback:
		HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		break;
	case EGfxBufferType::Vertex:
	case EGfxBufferType::Index:
	case EGfxBufferType::Storage:
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	CreateResource(&HeapProperties);
}

FGfxBuffer::~FGfxBuffer() = default;

void FGfxBuffer::Map(void** Pointer, const uint32 Start, const uint32 End) const
{
	const D3D12_RANGE Range = {
		.Begin = Start,
		.End = End
	};
	if (Range.End > Range.Begin)
	{
		RW_VERIFY_ID(Resource->Map(0, &Range, Pointer));
	}
	else
	{
		RW_VERIFY_ID(Resource->Map(0, nullptr, Pointer));
	}
}

void FGfxBuffer::Unmap(const uint32 Start, const uint32 End)
{
	const D3D12_RANGE Range = {
		.Begin = Start,
		.End = End
	};
	if (Range.End > Range.Begin)
	{
		Resource->Unmap(0, &Range);
	}
	else
	{
		Resource->Unmap(0, nullptr);
	}
	bIsDirty = true;
}

void FGfxBuffer::CopyMappedData(const void* Data, const size64 DataSize)
{
	void* MappedPointer = nullptr;
	Map(&MappedPointer);
	if (MappedPointer)
	{
		FMemory::Copy(MappedPointer, Data, DataSize);
	}
	Unmap();
}

void FGfxBuffer::SetData(void* NewData, const size64 NewSize)
{
	Data = NewData;
	AllocationSize = NewSize;
	bIsDirty = true;
}
