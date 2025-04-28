#pragma once

#include "GfxDescriptorHeap.hpp"

#include "Renderwerk/Graphics/GfxResource.hpp"

enum class ENGINE_API EGfxBufferType : uint8
{
	Vertex = 0,
	Index,
	Constant,
	Storage,
	Copy,
	Readback,
};

class ENGINE_API FGfxBuffer : public IGfxResource
{
public:
	explicit FGfxBuffer(FGfxDevice* InGfxDevice, EGfxBufferType InType, uint64 InDataSize, uint32 InStride);
	FGfxBuffer(FGfxDevice* InGfxDevice, EGfxBufferType InType, uint64 InDataSize, uint32 InStride, const FStringView& InDebugName);
	~FGfxBuffer() override;

	NON_COPY_MOVEABLE(FGfxBuffer)

public:
	void Map(void** Pointer, uint32 Start = 0, uint32 End = 0) const;
	void Unmap(uint32 Start = 0, uint32 End = 0);
	void CopyMappedData(void* CopyData, size64 DataSize);

	void SetData(void* NewData, size64 NewSize = 0);

public:
	[[nodiscard]] EGfxBufferType GetType() const { return Type; }

	[[nodiscard]] void* GetData() const { return Data; }
	[[nodiscard]] size64 GetDataSize() const { return DataSize; }
	[[nodiscard]] uint32 GetStride() const { return Stride; }

	[[nodiscard]] FGfxDescriptorHandle GetSRVDescriptorHandle() const { return SRVDescriptorHandle; }

	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return VertexBufferView; }
	[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return IndexBufferView; }

private:
	EGfxBufferType Type;

	void* Data;
	size64 DataSize;
	uint32 Stride;

	FGfxDescriptorHandle SRVDescriptorHandle;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW IndexBufferView = {};
};
