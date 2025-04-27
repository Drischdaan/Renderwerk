#include "pch.hpp"

#include "Renderwerk/Graphics/RenderPass/GfxRenderPass.hpp"

IGfxRenderPass::IGfxRenderPass(FGfxDevice* InGfxDevice)
	: IGfxRenderPass(InGfxDevice, TEXT("UnnamedRenderPass"))
{
}

IGfxRenderPass::IGfxRenderPass(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName)
{
}

IGfxRenderPass::~IGfxRenderPass()
{
}
