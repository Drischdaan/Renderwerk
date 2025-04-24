#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/Resource.hpp"

enum class ENGINE_API EBufferType : uint8
{
	Vertex = 0,
	Index,
	Constant,
	Storage,
	Copy,
	Readback,
};

class ENGINE_API FBuffer : public IResource
{
public:
	FBuffer(FDevice* InDevice, EBufferType InType, uint64 InSize, uint64 InStride);
	~FBuffer() override;

	NON_COPY_MOVEABLE(FBuffer)

public:
	void Map(void** Pointer, uint32 Start = 0, uint32 End = 0) const;
	void Unmap(uint32 Start = 0, uint32 End = 0) const;
	void CopyMappedData(const void* Data, size64 DataSize) const;

public:
	[[nodiscard]] EBufferType GetType() const { return Type; }

private:
	EBufferType Type;
	uint64 Stride;
};
