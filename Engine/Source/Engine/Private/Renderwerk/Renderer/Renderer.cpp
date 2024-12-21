#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;

	const FGraphicsBackendDesc BackendDesc = {};
	GraphicsBackend = MakeUnique<FGraphicsBackend>();
	GraphicsBackend->Initialize(BackendDesc);
}

void FRenderer::Destroy()
{
	GraphicsBackend->Destroy();
	GraphicsBackend.reset();
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
