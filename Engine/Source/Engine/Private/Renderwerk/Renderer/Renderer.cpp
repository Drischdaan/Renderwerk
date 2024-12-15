#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/VulkanGraphicsApi.h"
#include "Renderwerk/Platform/Window.h"

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

void FRenderer::Resize() const
{
	GraphicsApi->Resize();
}

void FRenderer::BeginFrame() const
{
	GraphicsApi->BeginFrame();
}

void FRenderer::EndFrame() const
{
	GraphicsApi->EndFrame();
}
