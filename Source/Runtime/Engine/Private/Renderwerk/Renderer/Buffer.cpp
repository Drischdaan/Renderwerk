#include "pch.hpp"

#include "Renderwerk/Renderer/Buffer.hpp"

FBuffer::FBuffer(FDevice* InDevice, const EBufferType InType, const uint64 InSize, const uint64 InStride)
	: IResource(InDevice), Type(InType), Stride(InStride)
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
	if (Type == EBufferType::Storage)
	{
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	CD3DX12_HEAP_PROPERTIES HeapProperties = {};
	switch (Type)
	{
	case EBufferType::Constant:
	case EBufferType::Copy:
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case EBufferType::Readback:
		HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		break;
	case EBufferType::Vertex:
	case EBufferType::Index:
	case EBufferType::Storage:
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	CreateResource(&HeapProperties);
}

FBuffer::~FBuffer()
{
}

void FBuffer::Map(void** Pointer, const uint32 Start, const uint32 End) const
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

void FBuffer::Unmap(const uint32 Start, const uint32 End) const
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
}

void FBuffer::CopyMappedData(const void* Data, const size64 DataSize) const
{
	void* MappedPointer = nullptr;
	Map(&MappedPointer);
	if (MappedPointer)
	{
		FMemory::Copy(MappedPointer, Data, DataSize);
	}
	Unmap();
}
