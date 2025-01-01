#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsBackend.h"
#include "Renderwerk/Graphics/GraphicsWindowContext.h"
#include "Renderwerk/Platform/Window.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;

	RW_LOG(LogRenderer, Info, "Using '{}' graphics backend", GetEnumValueName(Description.BackendType));
	Description.Window->AppendTitle(std::format(" <{}>", GetEnumValueName(Description.BackendType)).c_str());

	FGraphicsBackendDesc BackendDesc;
	BackendDesc.bEnableDebugging = true;
	BackendDesc.bEnableGpuValidation = true;
	GraphicsBackend = IGraphicsBackend::Create(Description.BackendType);
	GraphicsBackend->Initialize(BackendDesc);

	WindowContext = GraphicsBackend->CreateWindowContext();
	WindowContext->Initialize(Description.Window);

	// TODO: Retrieve this from config system
	constexpr uint32 AdapterDeviceId = 0;
	TSharedPtr<IGraphicsAdapter> GraphicsAdapter = GetAdapter(AdapterDeviceId);
}

void FRenderer::Destroy()
{
	WindowContext->Destroy();
	WindowContext.reset();
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

TSharedPtr<IGraphicsAdapter> FRenderer::GetAdapter(uint32 AdapterDeviceId) const
{
	TSharedPtr<IGraphicsAdapter> GraphicsAdapter;
	if (AdapterDeviceId == 0)
	{
		RW_LOG(LogRenderer, Info, "Selecting suitable adapter...");
		TVector<TSharedPtr<IGraphicsAdapter>> GraphicsAdapters = GraphicsBackend->GetAvailableAdapters();
		GraphicsAdapter = SelectSuitableAdapter(GraphicsAdapters, WindowContext);
		// TODO: Set device id in config system
	}
	else
	{
		RW_LOG(LogRenderer, Info, "Using adapter with DeviceId: {:#x}", AdapterDeviceId);
		GraphicsAdapter = GraphicsBackend->GetAdapterByDeviceId(AdapterDeviceId);
		if (GraphicsAdapter == nullptr)
		{
			RW_LOG(LogRenderer, Warning, "Adapter with DeviceId: {:#x} not found. Trying to select suitable...", AdapterDeviceId);
			TVector<TSharedPtr<IGraphicsAdapter>> GraphicsAdapters = GraphicsBackend->GetAvailableAdapters();
			GraphicsAdapter = SelectSuitableAdapter(GraphicsAdapters, WindowContext);
		}
	}
	VERIFY(GraphicsAdapter != nullptr, "Failed to select suitable adapter");
	FGraphicsAdapterProperties Properties = GraphicsAdapter->GetProperties();
	RW_LOG(LogRenderer, Info, "Selected Adapter: {}", Properties.Name);
	RW_LOG(LogRenderer, Info, "\t- Type: {}", GetEnumValueName(Properties.Type));
	RW_LOG(LogRenderer, Info, "\t- DeviceId: {}", Properties.DeviceId);
	RW_LOG(LogRenderer, Info, "\t- Vendor: {}", GetGraphicsAdapterVendorName(Properties.Vendor));
	RW_LOG(LogRenderer, Info, "\t- DriverVersion: {}", GraphicsAdapter->GetDriverVersionString());
	return GraphicsAdapter;
}

TSharedPtr<IGraphicsAdapter> FRenderer::SelectSuitableAdapter(const TSpan<TSharedPtr<IGraphicsAdapter>>& GraphicsAdapters,
                                                              const TSharedPtr<IGraphicsWindowContext>& WindowContext)
{
	TSharedPtr<IGraphicsAdapter> SuitableAdapter;
	for (const TSharedPtr<IGraphicsAdapter>& Adapter : GraphicsAdapters)
	{
		Adapter->Initialize(WindowContext);
		const FGraphicsAdapterProperties Properties = Adapter->GetProperties();
		if (Properties.Type != EGraphicsAdapterType::Discrete)
			continue;
		SuitableAdapter = Adapter;
		break;
	}
	return SuitableAdapter;
}
