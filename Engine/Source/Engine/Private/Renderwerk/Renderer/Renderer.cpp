#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
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

	Surface = GraphicsBackend->CreateSurface(Description.Window);

	const TVector<FString> DeviceExtensions = {};

	TSharedPtr<FGraphicsAdapter> GraphicsAdapter;
	const TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsBackend->GetAdapters();
	for (const TSharedPtr<FGraphicsAdapter>& CurrentAdapter : Adapters)
	{
		CurrentAdapter->Initialize(Surface);
		if (GraphicsBackend->IsAdapterSuitable(CurrentAdapter, DeviceExtensions))
		{
			GraphicsAdapter = CurrentAdapter;
			break;
		}
	}
	ASSERT(GraphicsAdapter, "No suitable adapter found");
	RW_LOG(LogGraphics, Info, "|   Type   | Family Index | Queue Index |");
	RW_LOG(LogGraphics, Info, "|----------|--------------|-------------|");
	for (const auto& [Type, Metadata] : GraphicsAdapter->GetQueueMetadataMap())
	{
		FString QueueName = FString(GetEnumValueName(Type));
		const size64 Padding = 8 - QueueName.length();
		if (Padding > 0)
		{
			for (size64 Index = 0; Index < Padding; ++Index)
				QueueName += " ";
		}
		RW_LOG(LogGraphics, Info, "| {} |       {}      |      {}      |", QueueName, Metadata.FamilyIndex, Metadata.QueueIndex);
	}
}

void FRenderer::Destroy()
{
	GraphicsBackend->DestroySurface(Surface);
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
