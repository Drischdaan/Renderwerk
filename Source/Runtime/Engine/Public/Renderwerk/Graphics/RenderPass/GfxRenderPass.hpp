#pragma once

#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class ENGINE_API IGfxRenderPass : public IGfxDeviceChild
{
public:
	explicit IGfxRenderPass(FGfxDevice* InGfxDevice);
	IGfxRenderPass(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	virtual ~IGfxRenderPass() override;

	NON_COPY_MOVEABLE(IGfxRenderPass)

public:
	virtual void CreateResources(const TRef<FGfxResourceManager>& ResourceManager) = 0;

	virtual void ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, uint32 NewWidth, uint32 NewHeight)
	{
	}

	virtual void ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager) = 0;

	virtual void Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer) = 0;

private:
};
