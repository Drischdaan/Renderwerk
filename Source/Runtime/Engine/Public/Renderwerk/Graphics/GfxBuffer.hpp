#pragma once

#include "Renderwerk/Graphics/GfxResource.hpp"

class ENGINE_API FGfxBuffer : public IGfxResource
{
public:
	explicit FGfxBuffer(FGfxDevice* InGfxDevice);
	FGfxBuffer(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	~FGfxBuffer() override;

	NON_COPY_MOVEABLE(FGfxBuffer)

public:

private:
};
