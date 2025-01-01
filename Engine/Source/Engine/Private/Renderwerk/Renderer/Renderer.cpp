#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"
#include "Renderwerk/Platform/Window.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;

	RW_LOG(LogRenderer, Info, "Using '{}' graphics backend", GetEnumValueName(Description.BackendType));
	Description.Window->AppendTitle(std::format(" <{}>", GetEnumValueName(Description.BackendType)).c_str());

	FGraphicsBackendDesc BackendDesc = {};
	BackendDesc.bEnableDebugging = true;
	BackendDesc.bEnableGpuValidation = true;
	GraphicsBackend = IGraphicsBackend::Create(Description.BackendType);
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
