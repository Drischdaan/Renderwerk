#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/VulkanGraphicsAdapter.h"
#include "Renderwerk/Graphics/VulkanGraphicsApi.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	FVulkanGraphicsApiDesc GraphicsApiDesc = {};
	GraphicsApiDesc.Window = Description.Window;
	GraphicsApi = MakeUnique<FVulkanGraphicsApi>(GraphicsApiDesc);
}

FRenderer::~FRenderer()
{
	GraphicsApi.reset();
}
