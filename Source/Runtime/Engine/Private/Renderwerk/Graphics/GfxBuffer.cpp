#include "pch.hpp"

#include "Renderwerk/Graphics/GfxBuffer.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice, const EGfxBufferType InType, const uint64 InDataSize, const uint32 InStride)
	: FGfxBuffer(InGfxDevice, InType, InDataSize, InStride, TEXT("UnnamedBuffer"))
{
}

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice, const EGfxBufferType InType, const uint64 InDataSize, uint32 InStride, const FStringView& InDebugName)
	: IGfxResource(InGfxDevice, InDebugName), Type(InType), DataSize(InDataSize), Stride(InStride)
{
	AllocationSize = InDataSize;

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	if (Type == EGfxBufferType::Constant)
	{
		ResourceDesc.Width = (DataSize + 255) & ~255;
	}
	else
	{
		ResourceDesc.Width = DataSize;
	}
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
	Resource->SetName(GetDebugName().c_str());

	if (Type == EGfxBufferType::Vertex)
	{
		VertexBufferView.BufferLocation = Resource->GetGPUVirtualAddress();
		VertexBufferView.SizeInBytes = static_cast<uint32>(DataSize);
		VertexBufferView.StrideInBytes = Stride;
	}
	else if (Type == EGfxBufferType::Index)
	{
		IndexBufferView.BufferLocation = Resource->GetGPUVirtualAddress();
		IndexBufferView.SizeInBytes = static_cast<uint32>(DataSize);
		IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
	else if (Type == EGfxBufferType::Constant)
	{
		SRVDescriptorHandle = GfxDevice->GetSRVDescriptorHeap()->Allocate();

		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc = {};
		ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
		ConstantBufferViewDesc.SizeInBytes = static_cast<uint32>((DataSize + 255) & ~255);

		ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
		NativeDevice->CreateConstantBufferView(&ConstantBufferViewDesc, SRVDescriptorHandle.GetCPUHandle());
	}
}

FGfxBuffer::~FGfxBuffer()
{
	if (Type == EGfxBufferType::Constant)
	{
		GfxDevice->GetSRVDescriptorHeap()->Free(SRVDescriptorHandle);
	}
}

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
	if ((Type != EGfxBufferType::Constant) && (Type != EGfxBufferType::Copy))
	{
		bIsDirty = true;
	}
}

void FGfxBuffer::CopyMappedData(void* CopyData, const size64 InDataSize)
{
	void* MappedPointer = nullptr;
	Map(&MappedPointer);
	if (MappedPointer)
	{
		FMemory::Copy(MappedPointer, CopyData, InDataSize);
	}
	Unmap();
	SetData(CopyData, InDataSize);
}

void FGfxBuffer::SetData(void* NewData, const size64 NewSize)
{
	Data = NewData;
	AllocationSize = NewSize;
	if ((Type != EGfxBufferType::Constant) && (Type != EGfxBufferType::Copy))
	{
		bIsDirty = true;
	}
}
