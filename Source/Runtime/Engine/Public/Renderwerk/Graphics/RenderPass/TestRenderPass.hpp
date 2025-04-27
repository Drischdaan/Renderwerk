#pragma once

#include "Renderwerk/Graphics/RenderPass/GfxRenderPass.hpp"

class ENGINE_API FTestRenderPass : public IGfxRenderPass
{
public:
	explicit FTestRenderPass(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	~FTestRenderPass() override;

	NON_COPY_MOVEABLE(FTestRenderPass)

public:
	void CreateResources(const TRef<FGfxResourceManager>& ResourceManager) override;
	void ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, uint32 NewWidth, uint32 NewHeight) override;
	void ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager) override;

	void Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer) override;

private:
	void CreateRenderTarget(const TRef<FGfxResourceManager>& ResourceManager, uint32 Width, uint32 Height);

private:
	TRef<FGfxGraphicsPipeline> GraphicsPipeline;
	TRef<FGfxTexture> RenderTarget;
};
