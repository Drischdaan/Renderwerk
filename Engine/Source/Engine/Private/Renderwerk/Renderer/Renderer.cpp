#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
	GraphicsApi = IGraphicsApi::NewInstance(Description.GraphicsApiType);
}

FRenderer::~FRenderer()
{
	GraphicsApi.reset();
}
