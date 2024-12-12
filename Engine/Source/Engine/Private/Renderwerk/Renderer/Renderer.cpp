#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	GraphicsApi = IGraphicsApi::NewInstance(Description.GraphicsApiType);
	RW_LOG(LogRenderer, Info, "Renderer initialized with {}", GetEnumValueName(Description.GraphicsApiType));
}

FRenderer::~FRenderer()
{
	GraphicsApi.reset();
}
