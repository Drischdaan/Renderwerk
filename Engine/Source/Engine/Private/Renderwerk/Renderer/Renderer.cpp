#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/VulkanGraphicsApi.h"

namespace
{
	DEFINE_LOG_CHANNEL(LogRenderer);
}

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	FVulkanGraphicsApiDesc GraphicsApiDesc = {};
	GraphicsApiDesc.Window = Description.Window;
	GraphicsApi = MakeUnique<FVulkanGraphicsApi>(GraphicsApiDesc);

	TVector<TSharedPtr<FVulkanGraphicsAdapter>> Adapters = GraphicsApi->AcquireAdapters();
}

FRenderer::~FRenderer()
{
	GraphicsApi.reset();
}
