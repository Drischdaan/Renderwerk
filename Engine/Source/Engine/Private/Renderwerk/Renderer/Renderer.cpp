#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/VulkanGraphicsApi.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	GraphicsApi = MakeUnique<FVulkanGraphicsApi>();
}

FRenderer::~FRenderer()
{
	GraphicsApi.reset();
}
