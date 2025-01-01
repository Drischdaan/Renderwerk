#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;
}

void FRenderer::Destroy()
{
}

void FRenderer::Resize()
{
	PROFILE_FUNCTION();
}

void FRenderer::BeginFrame()
{
	PROFILE_FUNCTION();
}

void FRenderer::EndFrame()
{
	PROFILE_FUNCTION();
}
