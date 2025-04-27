#pragma once

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
	explicit FGfxBuffer(FGfxDevice* InGfxDevice, EGfxBufferType InType, uint64 InSize);
	FGfxBuffer(FGfxDevice* InGfxDevice, EGfxBufferType InType, uint64 InSize, const FStringView& InDebugName);
	~FGfxBuffer() override;

	NON_COPY_MOVEABLE(FGfxBuffer)

public:
	void Map(void** Pointer, uint32 Start = 0, uint32 End = 0) const;
	void Unmap(uint32 Start = 0, uint32 End = 0);
	void CopyMappedData(const void* Data, size64 DataSize);

	void SetData(void* NewData, size64 NewSize = 0);

public:
	[[nodiscard]] EGfxBufferType GetType() const { return Type; }

	[[nodiscard]] void* GetData() const { return Data; }

private:
	EGfxBufferType Type;

	void* Data;
};
