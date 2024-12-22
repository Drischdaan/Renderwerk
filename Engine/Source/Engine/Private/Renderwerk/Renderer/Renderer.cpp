#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsBackend.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

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

	TVector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	const TSharedPtr<FGraphicsAdapter> GraphicsAdapter = SelectAdapter(DeviceExtensions);
	GraphicsDevice = GraphicsBackend->CreateDevice(GraphicsAdapter);
	GraphicsDevice->Initialize(DeviceExtensions);
}

void FRenderer::Destroy()
{
	GraphicsDevice->Destroy();
	GraphicsDevice.reset();
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

TSharedPtr<FGraphicsAdapter> FRenderer::SelectAdapter(const TSpan<const char*>& RequiredExtensions) const
{
	TSharedPtr<FGraphicsAdapter> GraphicsAdapter;
	const TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsBackend->GetAdapters();
	for (const TSharedPtr<FGraphicsAdapter>& CurrentAdapter : Adapters)
	{
		CurrentAdapter->Initialize(Surface);
		if (GraphicsBackend->IsAdapterSuitable(CurrentAdapter, RequiredExtensions))
		{
			GraphicsAdapter = CurrentAdapter;
			break;
		}
	}
	ASSERT(GraphicsAdapter, "No suitable adapter found");
	RW_LOG(LogGraphics, Info, "Selected adapter: {}", GraphicsAdapter->GetProperties().Name);
	RW_LOG(LogGraphics, Info, "\t- Type: {}", GetEnumValueName(GraphicsAdapter->GetProperties().Type));
	RW_LOG(LogGraphics, Info, "\t- Vendor: {}", GetVendorString(GraphicsAdapter->GetProperties().Vendor));
	RW_LOG(LogGraphics, Info, "\t- Driver Version: {}", GraphicsAdapter->GetDriverVersionString());
	RW_LOG(LogGraphics, Info, "\t- Api Version: {}.{}.{}", VK_VERSION_MAJOR(GraphicsAdapter->GetProperties().ApiVersion),
	       VK_VERSION_MINOR(GraphicsAdapter->GetProperties().ApiVersion), VK_VERSION_PATCH(GraphicsAdapter->GetProperties().ApiVersion));
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
	return GraphicsAdapter;
}
