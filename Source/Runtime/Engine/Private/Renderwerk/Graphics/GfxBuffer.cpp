#include "pch.hpp"

#include "Renderwerk/Graphics/GfxBuffer.hpp"

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice)
	: FGfxBuffer(InGfxDevice, TEXT("UnnamedBuffer"))
{
}

FGfxBuffer::FGfxBuffer(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxResource(InGfxDevice, InDebugName)
{
}

FGfxBuffer::~FGfxBuffer() = default;
